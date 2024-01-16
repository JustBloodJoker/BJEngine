#include "Script.h"

namespace BJEngine
{
    size_t line = 0;
    void Script::ParseScript(char* fileName)
    {
        std::string dat = fileName;
        if (dat == "") return;

        file.open(fileName);

        if (!file.is_open())
        {
            Log::Get()->Err("Can't open script");
        } 
        else
        {
            this->fileName = fileName;
            parameters.clear();
            parsedFunctions.clear();
            parameterChanger.clear();
            parsed = false;
            try
            { 
                line = 0;
                while (1)
                {
                    functionS dataSet;
                    std::string tmp;

                    line++;
                    file >> tmp;

                    if (tmp == ENDS)
                    {
                        file.close();
                        for (const auto& entry : parameters) {
                            const std::string& key = entry.first;
                            float* newValue = entry.second;

                      
                            bparameters[key] = *newValue;
                        }

                        break;
                    }
                    else if (tmp == REG)
                    {
                        std::string name;
                        std::getline(file, tmp);
                        std::istringstream(tmp) >> name >> dat;
                        parameters[name] = new float(GetParameter(dat));
                        dat = "0";
                        
                    }
                    else if (tmp == HESH)
                    {
                        std::getline(file, tmp);
                        MathOperation(tmp);
                        
                    }
                    else if (tmp == OP)
                    {
                        std::getline(file, tmp);
                        FunctionReg(tmp);
                        
                    }
                    else
                    {
                        throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect first parameter"));
                    }
                }
            }
            catch (const std::invalid_argument& ex)
            {
                Log::Get()->Err(ex.what());
            }  
        }       
    }

    dx::XMMATRIX Script::DrawScript()
    {
        dx::XMMATRIX tmp = dx::XMMatrixIdentity();

        if(parsed)
            for (auto it = parsedFunctions.begin(); it != parsedFunctions.end(); ) 
            {
                auto& el = *it;
                tmp *= el.foo(*el.x, *el.y, *el.z);
    
                if (!el.rep) 
                {
                    it = parsedFunctions.erase(it); 
                }
                else 
                {
                ++it;
                }
            }

        for (auto& el : parameterChanger)
        {
            if (el.foo)
            {
                *el.param = el.foo(*el.val, *el.val2);
            }
        }

        return tmp;
    }

    void Script::Close()
    {
        parsedFunctions.clear();
    }

    float* Script::GetPointerParameter(std::string data)
    {
        float* param;

        auto it = parameters.find(data);
        if (it != parameters.end())
        {
            param = parameters.at(data);
        }
        else
        {
            param = new float();
            std::istringstream(data) >> *param;
        }

        return param;
    }

    const float Script::GetParameter(std::string data) const
    {
        float param;

        auto it = parameters.find(data);
        if (it != parameters.end())
        {
            param = *parameters.at(data);
        }
        else
        {
            std::istringstream(data) >> param;
        }

        return param;
    }

    void Script::MathOperation(std::string data)
    {
        std::string tmp, val1, val2, operation, isLoop;
        std::istringstream str(data);
        std::streampos cPos = str.tellg();
        str >> tmp;
        if (tmp == EQU)
        {
            str >> tmp;
            cPos = str.tellg();
        } 
        else
        {
            str.seekg(cPos);
            tmp = "";
        }
        
        str >> operation;
        //if (!str.good())
        //{
        //   throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect math operation"));
        //}

        try
        {

            if (IsTanSinCos(operation))
            {
                str >> val1;

               // if (str.good())
               // {
               //     throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect math operation"));
               // }

                if (tmp == "")
                    tmp = val1;

                if (IsRegistered(tmp) == parameters.end())
                {
                    throw std::invalid_argument(Script::ExeptionTextCreator("Can't find equ or value 1 parameter in registered parameters"));
                }

                *parameters[tmp] = TrigonometricOperationRes(operation, GetParameter(val1));
                
                
            }
            else if (operation == "log")
            {
                str >> val1 >> val2;
                //if (!str.good())
                //{
                //   throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect math operation"));
                //}

                if (tmp == "")
                    tmp = val1;

                if (val2 == "")
                {
                    val2 = std::to_string(exp(1));
                }

                if (GetParameter(val1) <= 0 || GetParameter(val2) <= 0)
                {
                    throw std::invalid_argument(Script::ExeptionTextCreator("Log from value <"));
                }

                if (IsRegistered(tmp) == parameters.end())
                {
                    throw std::invalid_argument(Script::ExeptionTextCreator("Can't find equ or value 1 parameter in registered parameters"));
                }

                
                *parameters[tmp] = log(GetParameter(val2)) / log(GetParameter(val1));
            }
            else if (operation == "abs")
            {
                str >> val1;

                if (tmp == "")
                    tmp = val1;

                if (IsRegistered(tmp) == parameters.end())
                {
                    throw std::invalid_argument(Script::ExeptionTextCreator("Can't find equ or value 1 parameter in registered parameters"));
                }

                *parameters[tmp] = fabs(GetParameter(val1));
            }
            else
            {
                str.seekg(cPos);
                str >> val1 >> operation >> val2;
                
                //if (str.good())
                //{
                //   throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect math operation"));
                //}

                if (tmp == "")
                    tmp = val1;

                if (IsRegistered(tmp) == parameters.end())
                {
                    throw std::invalid_argument(Script::ExeptionTextCreator("Can't find equ or value 1 parameter in registered parameters"));
                }

                *parameters[tmp] = MathSimpleOperationRes(operation, GetParameter(val1), GetParameter(val2));

            }
           
            str >> isLoop;
            if (isLoop == LOOP)
            {
                
                paramsC psr;
                psr.foo = GetMathFucntion(operation);
                psr.param = parameters[tmp];
                psr.val = GetPointerParameter(val1);
                psr.val2 = GetPointerParameter(val2);
                parameterChanger.push_back(psr);
            }

        }
        catch (const std::invalid_argument& ex)
        {
            Log::Get()->Err(ex.what());
        }
    }

    void Script::FunctionReg(std::string data)
    {
        functionS dataSet;
        std::istringstream str(data);
        std::string tmp;

        str >> tmp;
        if (tmp == ONCE)
        {
            dataSet.rep = false;
        }
        else if (tmp == LOOP)
        {
            dataSet.rep = true;
        }
        else
        {
            throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect repeat parameter"));
        }

        str >> tmp;
        if (tmp == MOVE)
        {
            dataSet.foo = SCRIPTS_DEF::Move;
        }
        else if (tmp == ROTATE)
        {
            dataSet.foo = SCRIPTS_DEF::Rotate;
        }
        else
        {
            throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect function parameter"));
        }

        str >> tmp;
        if (tmp == X)
        {
            str >> tmp;
            dataSet.x = GetPointerParameter(tmp);
        }
        else if (tmp == Y)
        {
            str >> tmp;
            dataSet.y = GetPointerParameter(tmp);
        }
        else if (tmp == Z)
        {
            str >> tmp;
            dataSet.z = GetPointerParameter(tmp);
        }
        else if (tmp == A)
        {
            str >> tmp;
            dataSet.x = GetPointerParameter(tmp);
            str >> tmp;
            dataSet.y = GetPointerParameter(tmp);
            str >> tmp;
            dataSet.z = GetPointerParameter(tmp);
        }
        else
        {
            throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect parameter type"));
        }
        parsed = true;
        parsedFunctions.push_back(dataSet);
    }

    auto Script::IsRegistered(std::string value) -> std::map<std::string, float*>::iterator
    {
        return parameters.find(value);
    }

    const float Script::MathSimpleOperationRes(std::string operation, float val1, float val2) const
    {
        float res;
        try
        {
            
            if (operation == "+")
            {
                res = val1 + val2;
            }
            else if (operation == "-")
            {
                res = val1 - val2;
            }
            else if (operation == "*")
            {
                res = val1 * val2;
            }
            else if (operation == "/")
            {
                res = val1 / val2;
            }
            else if (operation == "**")
            {
                res = std::pow(val1, val2);
            }
            else if (operation == "%")
            {
                res = static_cast<int>(val1) % static_cast<int>(val2);
            }
            else
            {
                throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect math operation"));
            }
        }
        catch (const std::invalid_argument& ex)
        {
            Log::Get()->Err(ex.what());
            return 0.0;
        }
        return res;
    }

    const float Script::TrigonometricOperationRes(std::string operation, float val1) const
    {
        float res;
        try
        {
            if (operation == "cos")
            {
                res = cos(val1);
            }
            else if (operation == "sin")
            {
                res = sin(val1);
            }
            else if (operation == "tan")
            {
                res = tan(val1);
            }
            else if (operation == "atan")
            {
                res = atan(val1);
            }
            else if (operation == "cotan")
            {
                res = 1 / tan(val1);
            }
            else if (operation == "acotan")
            {
                res = atan(1 / val1);
            }
            else if (operation == "acos")
            {
                res = acos(val1);
            }
            else if (operation == "asin")
            {
                res = asin(val1);
            }
            else
            {
                throw std::invalid_argument(Script::ExeptionTextCreator("Incorrect math operation"));
            }
        }
        catch (const std::invalid_argument& ex)
        {
            Log::Get()->Err(ex.what());
            return 0.0;
        }
        return res;
    }

    float(*Script::GetMathFucntion(std::string operation))(float, float)
    {
        if (operation == "cos")
        {
            return SCRIPTS_DEF::cos;
        }
        else if (operation == "sin")
        {
            return SCRIPTS_DEF::sin;
        }
        else if (operation == "tan")
        {
            return SCRIPTS_DEF::tan;
        }
        else if (operation == "atan")
        {
            return SCRIPTS_DEF::atan;
        }
        else if (operation == "acos")
        {
             return SCRIPTS_DEF::acos;
        }
        else if (operation == "asin")
        {
            return SCRIPTS_DEF::asin;
        }
        else if (operation == "log")
        {
            return SCRIPTS_DEF::log;
        }
        else if (operation == "ln")
        {
            return SCRIPTS_DEF::ln;
        }
        else if (operation == "+")
        {
            return SCRIPTS_DEF::sum;
        }
        else if (operation == "-")
        {
            return SCRIPTS_DEF::sub;
        }
        else if (operation == "*")
        {
            return SCRIPTS_DEF::mul;
        }
        else if (operation == "/")
        {
            return SCRIPTS_DEF::div;
        }
        else if (operation == "**")
        {
            return SCRIPTS_DEF::pow;
        }
        else if (operation == "%")
        {
            return SCRIPTS_DEF::remnant;
        }
        else
        {
            throw std::invalid_argument(Script::ExeptionTextCreator("Can't loop this operation"));
        }

        return nullptr;
    }

    bool Script::IsTanSinCos(std::string data)
    {
        return data == "tan" || data == "sin" || data == "cos" || data == "cotan" 
            || data == "atan"|| data == "acotan"|| data == "asin" || data == "acos";
    }

    bool Script::IsLog(std::string data)
    {
        return data == "log" || data == "ln";
    }

    std::string Script::ExeptionTextCreator(std::string text)
    {
        return text + std::string(" Line: ") + std::to_string(line);
    }

    Script::paramsC Script::ParseParameterChanger(std::string operation, std::string val1, std::string val2, std::string res)
    {
        paramsC psr;
        psr.foo = GetMathFucntion(operation);
        psr.param = parameters[res];
        psr.val = GetPointerParameter(val1);
        return psr;
    }

    void Script::ResetParameters()
    {
        for (const auto& entry : bparameters) {
            const std::string& key = entry.first;
            float value = entry.second;
            *parameters[key] = value;
        }

    }

    std::string Script::GetName() const
    {
        return fileName;
    }

    namespace SCRIPTS_DEF
    {

        dx::XMMATRIX Rotate(float x, float y, float z)
        {
            return dx::XMMatrixRotationX(x) * dx::XMMatrixRotationY(y) * dx::XMMatrixRotationZ(z);
        }

        dx::XMMATRIX Move(float x, float y, float z)
        {
            return dx::XMMatrixTranslation(x,y,z);
        }

        float tan(float val1, float val2)
        {
            return std::tan(val1);
        }

        float atan(float val1, float val2)
        {
            return std::atan(val1);
        }

        float cotan(float val1, float val2)
        {
            return 1/std::tan(val1);
        }

        float acotan(float val1, float val2)
        {
            return std::tan(1/val1);
        }

        float acos(float val1, float val2)
        {
            return std::acos(val1);
        }

        float cos(float val1, float val2)
        {
            return std::cos(val1);
        }

        float sin(float val1, float val2)
        {
            return std::sin(val1);
        }

        float asin(float val1, float val2)
        {
            return std::asin(val1);
        }

        float log(float val1, float val2)
        {
            return std::log(val2) / std::log(val1);
        }

        float ln(float val1, float val2)
        {
            return std::log(val1) / std::log(exp(1));
        }

        float abs(float val1, float val2)
        {
            return fabs(val1);
        }

        float sum(float val1, float val2)
        {
            return val1 + val2;
        }

        float mul(float val1, float val2)
        {
            return val1 * val2;
        }

        float div(float val1, float val2)
        {
            return val1 / val2;
        }

        float sub(float val1, float val2)
        {
            return val1 - val2;
        }

        float pow(float val1, float val2)
        {
            return pow(val1, val2);
        }

        float remnant(float val1, float val2)
        {
            return static_cast<int>(val1) % static_cast<int>(val2);
        }

    }
}


