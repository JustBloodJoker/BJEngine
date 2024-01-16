#include "UI.h"
#include "LightMananger.h"



namespace BJEngine
{
    ID3D11Device* UI::pd3dDevice = nullptr;
    Render* UI::render = nullptr;

    bool UI::ISINIT = false;
    
    ImGuiIO UI::io;
    
    bool  UI::unpack        = false;
    bool  UI::save          = false;
    bool  UI::saveas        = false;
    bool  UI::log           = false;
    bool  UI::autoScroll    = true;
    bool  UI::console       = false;
    bool* UI::isMenuOpen    = new bool(false);
    bool  UI::addElement    = true;
    bool  UI::mainWnd       = true;
    bool  UI::iscam         = false;
    bool  UI::ismodel       = false;
    bool  UI::islight       = false;
    bool  UI::addLight      = false;
    bool  UI::addMenuOpen   = false;
    bool  UI::addCamera     = false;
    bool  UI::creatingState = true;
    bool  UI::gameState     = false;
    bool  UI::cameraMoveUI  = false;
    bool  UI::addModel      = false;
    bool  UI::addSkyBox     = false;
    bool  UI::addSound      = false;


    ImVector<int> UI::LineOffsets;
    ImGuiTextBuffer UI::buff;

    std::vector<BJEngine::Camera*>* UI::camera = nullptr;
    std::string UI::namesCams = "";
    size_t  UI::focusedCamera = 1;
    int UI::currItemComboCams = 0;
 
    LightDesc* UI::pArrayLightDesc = nullptr;
    size_t UI::focusedLight = 0;
    std::string UI::namesLights = "";
    int UI::currItemComboLights = 0;
    size_t UI::indexLight = 0;

    std::vector<BJEngine::Model*> UI::models;
    size_t UI::focusedModel = 0;
    std::string UI::namesModels = "";
    int UI::currItemComboModels = 0;

    std::vector<Textures*> UI::textures_UI;
    int UI::currLightAddItem = 0;

    bool UI::Init(const HWND& hwnd, ID3D11Device* pd3dDevice,ID3D11DeviceContext* pImmediateContext, 
        std::vector<BJEngine::Camera*>& pCams, Render* render)
    {
        UI::pd3dDevice = pd3dDevice;
        UI::render = render;

        InitSomeTextureForGUI(pd3dDevice);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = ImGui::GetIO();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(pd3dDevice, pImmediateContext);
        ImGui::StyleColorsDark();
        
        camera = &pCams;
        ISINIT = true;
       
        ShowWindow(GetConsoleWindow(), SW_HIDE);

        pArrayLightDesc = LightMananger::lightDescBuffer.light;

        return true;
    }

    bool UI::Begin()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        


        HotKeys();
        MenuBar();





        ParseBooleans();

        return true;
    }

    bool UI::End()
    {
        

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


        return true;
    }

    void UI::AddLog(const char* ltext, const char* text)
    {
        if (ISINIT)
        {
            int oSize = buff.size();

            buff.append(ltext);
            buff.append(text);
            buff.append("\n");

            for (int nSize = buff.size(); oSize < nSize; oSize++)
            {
                if (buff[oSize] == '\n')
                    LineOffsets.push_back(oSize + 1);
            }
        }
    }

    void UI::Close()
    {
        LineOffsets.clear();

        for (auto& el : textures_UI)
        {
            el->Close();
        }
        textures_UI.clear();
        render = nullptr;
        pd3dDevice = nullptr;
    }

    bool UI::IsInit()
    {
        return ISINIT;  
    }

    const size_t UI::FocusedCamera()
    {   
        if(cameraMoveUI)
            return focusedCamera;

        return 0;
    }

    bool UI::AddModelToList(BJEngine::Model* model)
    {
        models.push_back(model);
        std::string end = "";
        if (namesModels.find(models[models.size() - 1]->filename) < namesModels.size())
        {
            end += " -copy";
        }
        end += '\0';
        namesModels += models[models.size() - 1]->filename + end;
        
        return true;
    }

    bool UI::ParseBooleans()
    {
        if (cameraMoveUI)
        {
            CameraMicroMove();
        }
        else
        {
            MainWindow();
            LogWindow();
            AddElementsWindow();


            //TEMP MENU
            Unpack();
            Pack();
            AddElementMenu();
            CreateLight();
            CreateCamera();
            CreateModel();
            CreateSkyBox();
            CreateSound();
        }

        return true;
    }

    bool UI::IsMenuOpen(bool& check)
    {
        if (isMenuOpen != &check && *isMenuOpen)
        {
            Log::Get()->Print("Can't open other window now!");
            check = false;
            return false;
        }
        UI::isMenuOpen = &check;
        return true;
    }

    bool UI::HotKeys()
    {
        if (Input::Get()->CheckKeyState(DIK_LCONTROL) && Input::Get()->CheckKeyState(DIK_O, true))
            unpack = true;

        else if (Input::Get()->CheckKeyState(DIK_LCONTROL) && Input::Get()->CheckKeyState(DIK_LSHIFT) && Input::Get()->CheckKeyState(DIK_S, true))
            saveas = true;

        else if (Input::Get()->CheckKeyState(DIK_LCONTROL) && Input::Get()->CheckKeyState(DIK_S, true))
            if (PackMananger::Get()->IsPath())
                save = true;
            else
                saveas = true;

        else if (Input::Get()->CheckKeyState(DIK_LCONTROL) && Input::Get()->CheckKeyState(DIK_LSHIFT) && Input::Get()->CheckKeyState(DIK_L, true))
            ConsoleWindow();

        else if (Input::Get()->CheckKeyState(DIK_LCONTROL) && Input::Get()->CheckKeyState(DIK_L, true))
            UI::log = true;

        else if (Input::Get()->CheckKeyState(DIK_LCONTROL) && Input::Get()->CheckKeyState(DIK_E, true))
            addElement = true;

        else if (Input::Get()->CheckKeyState(DIK_LCONTROL) && Input::Get()->CheckKeyState(DIK_M, true))
            mainWnd = true;

        return true;
    }

    bool UI::MenuBar()
    {

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "CTRL+O"))
                {
                    unpack = !unpack;
                }

                if (ImGui::MenuItem("Save", "CTRL+S"))
                {
                    if (PackMananger::Get()->IsPath())
                        save = !save;
                    else
                        saveas = !saveas;
                }
                if (ImGui::MenuItem("Save as", "CTRL+SHIFT+S"))
                {
                    saveas = !saveas;

                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Log", "CTRL+L"))
                {
                    UI::log = !UI::log;
                }
                if (ImGui::MenuItem("Debug Console", "CTRL+SHIFT+L"))
                {
                    ConsoleWindow();
                }
                if (ImGui::MenuItem("Element Window", "CTRL+E"))
                {
                    addElement = !addElement;
                }
                if (ImGui::MenuItem("Main Window", "CTRL+M"))
                {
                    mainWnd = !mainWnd;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

       

        return true;
    }

    bool UI::InitSomeTextureForGUI(ID3D11Device* pd3dDevice)
    {
        textures_UI.resize(8);

        textures_UI[UI_TEXTURES_ADDBUTTON] = new Textures(UI_TEXTURE_ADDELEMENT_PATH);
        textures_UI[UI_TEXTURES_ADDBUTTON]->InitTextures(pd3dDevice);

        textures_UI[UI_TEXTURES_ADDLIGHTBUTTON] = new Textures(UI_TEXTURE_LAMP_PATH);
        textures_UI[UI_TEXTURES_ADDLIGHTBUTTON]->InitTextures(pd3dDevice);

        textures_UI[UI_TEXTURES_ADDCAMERABUTTON] = new Textures(UI_TEXTURE_CAMERA_PATH);
        textures_UI[UI_TEXTURES_ADDCAMERABUTTON]->InitTextures(pd3dDevice);

        textures_UI[UI_TEXTURES_ADDMODELBUTTON] = new Textures(UI_TEXTURE_MODEL_PATH);
        textures_UI[UI_TEXTURES_ADDMODELBUTTON]->InitTextures(pd3dDevice);

        textures_UI[UI_TEXTURES_ADDSKYBOXBUTTON] = new Textures(UI_TEXTURE_SKYBOX_PATH);
        textures_UI[UI_TEXTURES_ADDSKYBOXBUTTON]->InitTextures(pd3dDevice);

        textures_UI[UI_TEXTURES_ADDSOUNDBUTTON] = new Textures(UI_TEXTURE_SOUND_PATH);
        textures_UI[UI_TEXTURES_ADDSOUNDBUTTON]->InitTextures(pd3dDevice);


        return false;
    }

    bool UI::Unpack()
    {
        if (unpack)
        {
            if (!IsMenuOpen(unpack))
            {
                return false;
            }

            if (ImGui::Begin("Open", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
            {
                
                ImGui::SetWindowPos("Open", ImVec2(BJEUtils::GetWindowWidth() / 2 - UI_OPENMENU_WIDTH / 2, BJEUtils::GetWindowHeight() / 2 - UI_OPENMENU_HEIGHT / 2));
                ImGui::SetWindowSize("Open", ImVec2(UI_OPENMENU_WIDTH, UI_OPENMENU_HEIGHT));

                static char pathto[100] = "C:\\";

                ImGui::InputText("Input path", pathto, 100);

                if (ImGui::Button("Open", ImVec2(50, 30)))
                {
                    UnpackMananger::Get()->Init(pathto);
                    unpack = false;
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(50, 30)))
                {
                    unpack = false;
                }

                ImGui::End();
            }
        }
        return true;
    }

    bool UI::Pack()
    {
        if (save)
        {
            PackMananger::Get()->Init();
            save = false;
        }
        else if (saveas)
        {
            if (!IsMenuOpen(saveas))
            {
                return false;
            }

            ImGui::Begin("Save", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);

            ImGui::SetWindowSize("Save", ImVec2(UI_OPENMENU_WIDTH, UI_OPENMENU_HEIGHT));
            ImGui::SetWindowPos("Save", ImVec2(BJEUtils::GetWindowWidth() / 2 - ImGui::GetWindowSize().x / 2, BJEUtils::GetWindowHeight() / 2 - ImGui::GetWindowSize().y / 2));
            
            static char pathto[100] = "C:\\";

            ImGui::InputText("Input path", pathto, 100);

            if (ImGui::Button("Yes", ImVec2(50, 30)))
            {
                PackMananger::Get()->Init(pathto);
                saveas = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("No", ImVec2(50, 30)))
            {
                saveas = false;
            }

            ImGui::End();
        }

        return true;
    }

    bool UI::LogWindow()
    {
        if (UI::log)
        {
            ImGui::Begin("Log", &UI::log, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
            {
                ImGui::SetWindowSize("Log", ImVec2(UI_LOGMENU_WIDTH, UI_LOGMENU_HEIGHT));
                ImGui::SetWindowPos("Log", ImVec2(0, BJEUtils::GetWindowHeight() - ImGui::GetWindowSize().y));
                
                
                ImGui::Checkbox("AutoScroll", &autoScroll);

                ImGui::Separator();

                ImGui::BeginChild("ggs");

                const char* buffBegin = buff.begin();
                const char* buffEnd = buff.end();

                for (int ln = 0; ln < LineOffsets.Size; ln++)
                {
                    const char* ls = buffBegin + LineOffsets[ln];
                    const char* le = (ln + 1 < LineOffsets.Size) ? (buffBegin + LineOffsets[ln + 1] - 1) : buffEnd;
                    ImGui::TextUnformatted(ls, le);
                }

                if ((autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                    ImGui::SetScrollHereY(1.0f);
                

                ImGui::EndChild();
                ImGui::End();
            }
            
        }  
        return true;
    }

    bool UI::ConsoleWindow()
    {
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        return true;
    }

    bool UI::MainWindow()
    {
        if (UI::mainWnd)
        {
            ImGui::Begin("Main", &UI::mainWnd, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);
            {
                ImGui::SetWindowSize("Main", ImVec2(UI_MAINMENU_WIDTH, UI_MAINMENU_HEIGHT));
                ImGui::SetWindowPos("Main", ImVec2(0, 15));

                
                if(ImGui::BeginMenuBar())
                {
                    if (ImGui::MenuItem("Models", NULL, &ismodel))
                    {
                        islight = false;
                        iscam = false;
                    }
                    if (ImGui::MenuItem("Lights", NULL, &islight))
                    {
                        ismodel = false;
                        iscam = false;
                    }
                    if (ImGui::MenuItem("Camera", NULL, &iscam))
                    {
                        ismodel = false;
                        islight = false;
                    }

                        
                    
                    ImGui::EndMenuBar();
                }

                if (ismodel)
                {
                    UI::Model();
                }
                else if (iscam)
                {
                    UI::Camera();
                }
                else if (islight)
                {
                    Light();
                }
                else
                {
                    ImGui::Text("Select mode on the menu bar!");
                }



                ImGui::End();
            }
        }

        return true;
    }

    bool UI::AddElementsWindow()
    {
        if (UI::addElement)
        {
            ImGui::Begin("Elements", &UI::addElement, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
            {
                ImGui::SetWindowSize("Elements", ImVec2(UI_ADDELEMENTMENU_WIDTH, UI_ADDELEMENTMENU_HEIGHT));
                ImGui::SetWindowPos("Elements", ImVec2(BJEUtils::GetWindowWidth() - ImGui::GetWindowSize().x, BJEUtils::GetWindowHeight() - ImGui::GetWindowSize().y));

                if (ImGui::ImageButton(textures_UI[UI_TEXTURES_ADDBUTTON]->GetTexture(), ImVec2(100.0f, 100.0f)))
                {
                    addMenuOpen = true;
                }


                ImGui::End();
            }
        }

        return true;
    }

    bool UI::CreateLight()
    {
        if (addLight)
        {
            if (!IsMenuOpen(addLight))
            {
                return false;
            }

            if (ImGui::Begin("Create Light", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
            {
                LightDesc ldTemp;
                
                ImGui::SetWindowPos("Create Light", ImVec2(BJEUtils::GetWindowWidth() / 2 - UI_OPENMENU_WIDTH / 2, BJEUtils::GetWindowHeight() / 2 - UI_OPENMENU_HEIGHT / 2));
                ImGui::SetWindowSize("Create Light", ImVec2(UI_OPENMENU_WIDTH, UI_OPENMENU_HEIGHT));

                ImGui::Combo("Type of light", &currLightAddItem, UI_ADDELEMENTMENU_LIGHTADD_COMBO);

                ldTemp.lightType = currLightAddItem;
                
                if (ImGui::Button("Open", ImVec2(50, 30)))
                {
                    LightMananger::AddLight(ldTemp);
                    addLight = false;
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(50, 30)))
                {
                    addLight = false;
                }

                ImGui::End();
            }
        }
        return true;
    }

    bool UI::CreateCamera()
    {
        if (addCamera)
        {
            camera->push_back(new BJEngine::Camera(pd3dDevice));
            UI::namesCams += (std::to_string(camera->size() - 1) + '\0');
            addCamera = false;
        }


        return true;
    }

    bool UI::CreateModel()
    {
        if (addModel)
        {
            if (!IsMenuOpen(addModel))
            {
                return false;
            }

            if (ImGui::Begin("Create Model", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
            {
                ImGui::SetWindowPos("Create Model", ImVec2(BJEUtils::GetWindowWidth() / 2 - UI_OPENMENU_WIDTH / 2, BJEUtils::GetWindowHeight() / 2 - UI_OPENMENU_HEIGHT / 2));
                ImGui::SetWindowSize("Create Model", ImVec2(UI_OPENMENU_WIDTH, UI_OPENMENU_HEIGHT));

                static char path[100];
                static char prepath[100];
                ImGui::SeparatorText("Path to the scene");
                ImGui::InputText(" ", path, 100);
                ImGui::SeparatorText("Prefix for texture path (optional field)");
                ImGui::InputText("  ", prepath, 100);

                if (ImGui::Button("Open", ImVec2(50, 30)))
                {
                    BJEngine::Model* temp = new BJEngine::Model(path);
                    temp->SetTexturesPrefixPath(BJEUtils::charToWString(prepath));
                    render->InitObjs(temp);
                    
                    path[0] = '\0';
                    prepath[0] = '\0';
                    addModel = false;
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(50, 30)))
                {
                    path[0] = '\0';
                    prepath[0] = '\0';
                    addModel = false;
                }

                ImGui::End();
            }
        }
        return true;
    }

    bool UI::CreateSkyBox()
    {
        if (addSkyBox)
        {
            if (!IsMenuOpen(addSkyBox))
            {
                return false;
            }

            if (ImGui::Begin("Create SkyBox", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
            {
                ImGui::SetWindowPos("Create SkyBox", ImVec2(BJEUtils::GetWindowWidth() / 2 - UI_OPENMENU_WIDTH / 2, BJEUtils::GetWindowHeight() / 2 - UI_OPENMENU_HEIGHT / 2));
                ImGui::SetWindowSize("Create SkyBox", ImVec2(UI_OPENMENU_WIDTH, UI_OPENMENU_HEIGHT));

                static char path[100];
                ImGui::SeparatorText("Path to the .dds texture");
                ImGui::InputText(" ", path, 100);
                
                if (ImGui::Button("Open", ImVec2(50, 30)))
                {
                    render->AddSkyBox(std::string(path));
                   

                    path[0] = '\0';
                    addSkyBox = false;
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(50, 30)))
                {
                    path[0] = '\0';
                    addSkyBox = false;
                }

                ImGui::End();
            }
        }
        return true;
    }

    bool UI::CreateSound()
    {
        if (addSound)
        {
            if (!IsMenuOpen(addSound))
            {
                return false;
            }

            if (ImGui::Begin("Create Sound", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse))
            {
                ImGui::SetWindowPos("Create Sound", ImVec2(BJEUtils::GetWindowWidth() / 2 - UI_OPENMENU_WIDTH / 2, BJEUtils::GetWindowHeight() / 2 - UI_OPENMENU_HEIGHT / 2));
                ImGui::SetWindowSize("Create Sound", ImVec2(UI_OPENMENU_WIDTH, UI_OPENMENU_HEIGHT));

                static char path[100];
                ImGui::SeparatorText("Path to the sound");
                ImGui::InputText(" ", path, 100);

                if (ImGui::Button("Open", ImVec2(50, 30)))
                {
                    render->CreateSound(std::string(path));

                    path[0] = '\0';
                    addSound = false;
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(50, 30)))
                {
                    path[0] = '\0';
                    addSound = false;
                }

                ImGui::End();
            }
        }
        return true;
    }

    bool UI::Model()
    {
        if (!models.empty())
        {

            

            ImGui::SeparatorText("Focused model");
            
            ImGui::Combo("Model Focus", &currItemComboModels, namesModels.c_str(), IM_ARRAYSIZE(namesModels.c_str()));
            focusedModel = currItemComboModels;

            ImGui::SeparatorText("Model settings");

            if (ImGui::TreeNode("Textures"))
            {

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("ffdss"))
            {
                ImGui::Text("Suka blyat");
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Elements"))
            {
                static bool* selection = new bool[models[focusedModel]->elements.size()]();

                for (size_t ind = 0; ind < models[focusedModel]->elements.size(); ind++)
                {
                    if (ImGui::Selectable(models[focusedModel]->elements[ind]->GetName().c_str(), selection[ind]))
                    {
                        if (!Input::Get()->CheckKeyState(DIK_RCONTROL))
                            memset(selection, 0, models[focusedModel]->elements.size());
                        
                        selection[ind] ^= 1;
                    }
                    models[focusedModel]->elements[ind]->SetFocusState(selection[ind]);
                }




                ImGui::TreePop();
            }


        }
        else
        {
            ImGui::Text("Project doesn't have models!");
        }



        return false;
    }

    bool UI::Camera()
    {
        if ((*camera).size() > 1)
        {
            ImGui::SeparatorText("Focused camera");

            currItemComboCams = focusedCamera - 1;
            ImGui::Combo("Camera Focus", &currItemComboCams, namesCams.c_str(), IM_ARRAYSIZE(namesCams.c_str()));
            focusedCamera = currItemComboCams + 1;

            ImGui::SeparatorText("Camera Settings");
            
            ImGui::SliderFloat("FoV", &(*camera)[focusedCamera]->FoV, M_PI / 16, M_PI * 3 / 4, "%.2f");

            float position[3] = { (*camera)[focusedCamera]->eye.vector4_f32[0],(*camera)[focusedCamera]->eye.vector4_f32[1],(*camera)[focusedCamera]->eye.vector4_f32[2] };
            ImGui::InputFloat3("Position", position);
            (*camera)[focusedCamera]->SetPosition(position[0], position[1], position[2]);

            float rotation[2] = { (*camera)[focusedCamera]->camYaw, (*camera)[focusedCamera]->camPitch };
            ImGui::SliderFloat2("Rotation", rotation, -M_PI, M_PI);
            (*camera)[focusedCamera]->camYaw = rotation[0];
            (*camera)[focusedCamera]->camPitch = rotation[1];

            ImGui::Separator();

            if (ImGui::Button("Move focused camera"))
            {
                cameraMoveUI = true;
            }
        }
        else
        {
            ImGui::Text("Project doesn't have cameras!");
        }
        return true;
    }

    bool UI::Light()
    {
        if (!LightMananger::IsHaveLights())
        {
            ImGui::Text("Project doesn't have lights!");
            return false;
        }

        ImGui::SeparatorText("Focused light");

        for (; indexLight < LightMananger::lightDescBuffer.lightsCount; indexLight++)
        {
            namesLights +=  BJEUtils::ParseLightTypeToStr(pArrayLightDesc[indexLight].lightType)
                + " " + std::to_string(indexLight) + '\0';
        }
        ImGui::Combo("Light Focus", &currItemComboLights, namesLights.c_str(), IM_ARRAYSIZE(namesLights.c_str()));

        focusedLight = currItemComboLights;

        ImGui::Separator();

        ImGui::Checkbox("Enabled", (bool*)(&pArrayLightDesc[focusedLight].enabled));

        float* variables[3] = { &pArrayLightDesc[focusedLight].pos.x, &pArrayLightDesc[focusedLight].pos.y, &pArrayLightDesc[focusedLight].pos.z };
        ImGui::SeparatorText("Light Position");
        ImGui::SliderFloat("sfff", &pArrayLightDesc[focusedLight].pos.x, -1000.0f, 1000.0f);
        ImGui::InputFloat3("Pos", *variables, "%.2f");

        if (pArrayLightDesc[focusedLight].lightType == BJEUtils::SPOTLIGHT)
        {
            float* variables2[3] = { &pArrayLightDesc[focusedLight].dir.x, &pArrayLightDesc[focusedLight].dir.y, &pArrayLightDesc[focusedLight].dir.z };
            ImGui::SeparatorText("Light Direction");
            ImGui::SliderFloat3("Dir", *variables2, -1.0f, 1.0f, "%.4f");
        }

        float* variables3[3] = { &pArrayLightDesc[focusedLight].color.x, &pArrayLightDesc[focusedLight].color.y, &pArrayLightDesc[focusedLight].color.z };
        ImGui::SeparatorText("Light Color");
        ImGui::ColorEdit3("Color", *variables3);
        
        ImGui::SeparatorText("Light Attenuation");
        ImGui::SliderFloat("Constant" , &pArrayLightDesc[focusedLight].att.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Lineral"  , &pArrayLightDesc[focusedLight].att.y, 0.0f, 1.0f);
        ImGui::SliderFloat("Quadratic", &pArrayLightDesc[focusedLight].att.z, 0.0f, 1.0f);


        return true;
    }

    bool UI::AddElementMenu()
    {
        if (addMenuOpen)
        {

            if (!IsMenuOpen(addMenuOpen))
            {
                return false;
            }

            if (ImGui::Begin("Create element", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse ))
            {
                ImGui::SetWindowPos("Create element", ImVec2(BJEUtils::GetWindowWidth() / 2 - UI_OPENMENU_WIDTH / 2, BJEUtils::GetWindowHeight() / 2 - UI_OPENMENU_HEIGHT / 2));
                ImGui::SetWindowSize("Create element", ImVec2(UI_OPENMENU_WIDTH, UI_OPENMENU_HEIGHT));


                if (ImGui::ImageButton(textures_UI[UI_TEXTURES_ADDLIGHTBUTTON]->GetTexture(), ImVec2(30.0f, 30.0f)))
                {
                    addMenuOpen = false;
                    addLight = true;
                }
                
                ImGui::SetItemTooltip("Create Light");
                ImGui::SameLine();
                
                if(ImGui::ImageButton(textures_UI[UI_TEXTURES_ADDCAMERABUTTON]->GetTexture(), ImVec2(30.0f, 30.0f)))
                {
                    addMenuOpen = false;
                    addCamera = true;
                } 

                ImGui::SetItemTooltip("Create Camera");
                ImGui::SameLine();

                if (ImGui::ImageButton(textures_UI[UI_TEXTURES_ADDMODELBUTTON]->GetTexture(), ImVec2(30.0f, 30.0f)))
                {
                    addMenuOpen = false;
                    addModel = true;
                }

                ImGui::SetItemTooltip("Open Scene");
                ImGui::SameLine();

                if (ImGui::ImageButton(textures_UI[UI_TEXTURES_ADDSKYBOXBUTTON]->GetTexture(), ImVec2(30.0f, 30.0f)))
                {
                    addMenuOpen = false;
                    addSkyBox = true;
                }

                ImGui::SetItemTooltip("Create SkyBox");
                ImGui::SameLine();

                if (ImGui::ImageButton(textures_UI[UI_TEXTURES_ADDSOUNDBUTTON]->GetTexture(), ImVec2(30.0f, 30.0f)))
                {
                    addMenuOpen = false;
                    addSound = true;
                }

                ImGui::SetItemTooltip("Create Sound");


                if (ImGui::Button("Cancel", ImVec2(100,30)))
                {
                    addMenuOpen = false;
                }


                ImGui::End();
            }
        }

        return true;
    }

    bool UI::CameraMicroMove()
    {
        ImGui::Begin("MoveCamera",NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize
                                          | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove 
                                          | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground 
                                          | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);

        (*camera)[0]->moveCamera = false;
        (*camera)[focusedCamera]->moveCamera = true;

        if (Input::Get()->CheckKeyState(DIK_SPACE))
        {
            (*camera)[0]->moveCamera = true;
            (*camera)[focusedCamera]->moveCamera = false;
            cameraMoveUI = false;
        }
        
        ImGui::SetWindowSize("MoveCamera", ImVec2(UI_MOVECAMERAFACE_WIDTH, UI_MOVECAMERAFACE_HEIGHT));
        ImGui::SetWindowPos("MoveCamera", ImVec2(0,50));
        
        ImGui::SetWindowFontScale(2.0f);

        ImGui::Text("PRESS SPACE TO SAVE CHANGES");
        
        ImGui::SetWindowFontScale(1.0f);
        
        ImGui::SliderFloat("Speed ", &(*camera)[focusedCamera]->moveSpeed, 0.1f, 100.0f, "%.5f");
        ImGui::SliderFloat("Sensitivity ", &(*camera)[focusedCamera]->sensitivity, 0.00001f, 0.002f, "%.5f");

        ImGui::End();

        return true;
    }

}