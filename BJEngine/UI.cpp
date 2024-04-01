#include "UI.h"
#include "LightMananger.h"



namespace BJEngine
{
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
    bool  UI::isPostP       = false;

    ImVector<int> UI::LineOffsets;
    ImGuiTextBuffer UI::buff;

    std::vector<BJEngine::Camera*>* UI::camera = nullptr;
    std::string UI::namesCams = "";
    size_t  UI::focusedCamera = 1;
    int UI::currItemComboCams = 0;
 
    LightMananger* UI::lman = nullptr;
    size_t UI::focusedLight = 0;
    std::string UI::namesLights = "";
    int UI::currItemComboLights = 0;
    size_t UI::indexLight = 0;

    std::string UI::models = "";
    int UI::currModelCombo = 0;
    char UI::text[200] = "";
    dx::XMFLOAT3 UI::scale = dx::XMFLOAT3(1.0f, 1.0f, 1.0f);
    dx::XMFLOAT3 UI::rot = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
    dx::XMFLOAT3 UI::pos = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);

    std::vector<Textures*> UI::textures_UI;
    int UI::currLightAddItem = 0;



    std::unordered_map<BJEUtils::POST_PROCESSING, bool> UI::postProcessingBools;

    bool UI::Init(const HWND& hwnd, 
        std::vector<BJEngine::Camera*>& pCams, Render* render)
    {
        UI::render = render;

        InitSomeTextureForGUI();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = ImGui::GetIO();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(GP::GetDevice(), GP::GetDeviceContext());
        ImGui::StyleColorsDark();
        
        camera = &pCams;
        ISINIT = true;
       
        ShowWindow(GetConsoleWindow(), SW_HIDE);

       

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


    std::unordered_map<BJEUtils::POST_PROCESSING, bool>& UI::GetPostProcessingStatus() noexcept
    {
        return postProcessingBools;
    }

    void UI::AddModelString(std::string str)
    {
        models += str + '\0';
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

    bool UI::InitSomeTextureForGUI()
    {
        textures_UI.resize(8);

        textures_UI[UI_TEXTURES_ADDBUTTON] = new Textures(UI_TEXTURE_ADDELEMENT_PATH);
        textures_UI[UI_TEXTURES_ADDBUTTON]->InitTextures();

        textures_UI[UI_TEXTURES_ADDLIGHTBUTTON] = new Textures(UI_TEXTURE_LAMP_PATH);
        textures_UI[UI_TEXTURES_ADDLIGHTBUTTON]->InitTextures();

        textures_UI[UI_TEXTURES_ADDCAMERABUTTON] = new Textures(UI_TEXTURE_CAMERA_PATH);
        textures_UI[UI_TEXTURES_ADDCAMERABUTTON]->InitTextures();

        textures_UI[UI_TEXTURES_ADDMODELBUTTON] = new Textures(UI_TEXTURE_MODEL_PATH);
        textures_UI[UI_TEXTURES_ADDMODELBUTTON]->InitTextures();

        textures_UI[UI_TEXTURES_ADDSKYBOXBUTTON] = new Textures(UI_TEXTURE_SKYBOX_PATH);
        textures_UI[UI_TEXTURES_ADDSKYBOXBUTTON]->InitTextures();

        textures_UI[UI_TEXTURES_ADDSOUNDBUTTON] = new Textures(UI_TEXTURE_SOUND_PATH);
        textures_UI[UI_TEXTURES_ADDSOUNDBUTTON]->InitTextures();


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
                    render->LoadProject(std::string(pathto));
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
            //pack
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
            static char name[100] = "hehe.bje";

            ImGui::InputText("Input path", pathto, 100);
            ImGui::InputText("Name: ", name, 100);
            if (ImGui::Button("Yes", ImVec2(50, 30)))
            {
                render->SaveProject(std::string(pathto), std::string(name));
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
                        isPostP = false;
                    }
                    if (ImGui::MenuItem("Lights", NULL, &islight))
                    {
                        ismodel = false;
                        iscam = false;
                        isPostP = false;
                    }
                    if (ImGui::MenuItem("Camera", NULL, &iscam))
                    {
                        ismodel = false;
                        islight = false;
                        isPostP = false;
                    }
                    if (ImGui::MenuItem("Globals", NULL, &isPostP))
                    {
                        iscam = false;
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
                    UI::Light();
                }
                else if (isPostP)
                {
                    UI::PostProcessing();
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
                static BJEStruct::LightDesc ldTemp;
                static bool chBox = ldTemp.shadowEnabled;

                ImGui::SetWindowPos("Create Light", ImVec2(BJEUtils::GetWindowWidth() / 2 - UI_OPENMENU_WIDTH / 2, BJEUtils::GetWindowHeight() / 2 - UI_OPENMENU_HEIGHT / 2));
                ImGui::SetWindowSize("Create Light", ImVec2(UI_OPENMENU_WIDTH, UI_OPENMENU_HEIGHT));

                ImGui::Combo("Type of light", &currLightAddItem, UI_ADDELEMENTMENU_LIGHTADD_COMBO);

                ImGui::Checkbox("Shadow ", &chBox);

                ldTemp.shadowEnabled = chBox;
                ldTemp.lightType = currLightAddItem;
                
                if (ImGui::Button("Open", ImVec2(50, 30)))
                {
                    render->SetLight(ldTemp);
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
            camera->push_back(new BJEngine::Camera(GP::GetDevice()));
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
        if (models != "")
        {
            ImGui::Text("Model: ");
            if (ImGui::Combo("Light Focus", &currModelCombo, models.c_str(), IM_ARRAYSIZE(models.c_str())))
            {
                rot = render->GetDrawable(currModelCombo)->GetRotate();
                pos = render->GetDrawable(currModelCombo)->GetPos();
                scale = render->GetDrawable(currModelCombo)->GetScale();
            }

            ImGui::SeparatorText("Settings  ");

            ImGui::SeparatorText("Rotate");
            if (ImGui::SliderFloat("X ", &rot.x, 0.0f, 3.14f))
            {
                render->GetDrawable(currModelCombo)->SetRotateX(rot.x);
            }
            if (ImGui::SliderFloat("Y ", &rot.y, 0.0f, 3.14f))
            {
                render->GetDrawable(currModelCombo)->SetRotateY(rot.y);
            }
            if (ImGui::SliderFloat("Z ", &rot.z, 0.0f, 3.14f))
            {
                render->GetDrawable(currModelCombo)->SetRotateZ(rot.z);
            }

            ImGui::SeparatorText("Scale");
            if (ImGui::SliderFloat("X  ", &scale.x, 0.001f, 100.0f))
            {
                render->GetDrawable(currModelCombo)->SetScaleX(scale.x);
            }
            if (ImGui::SliderFloat("Y  ", &scale.y, 0.001f, 100.0f))
            {
                render->GetDrawable(currModelCombo)->SetScaleY(scale.y);
            }
            if (ImGui::SliderFloat("Z  ", &scale.z, 0.001f, 100.0f))
            {
                render->GetDrawable(currModelCombo)->SetScaleZ(scale.z);
            }

            ImGui::SeparatorText("Position");
            if (ImGui::SliderFloat("X   ", &pos.x, -100.0f, 100.0f))
            {
                render->GetDrawable(currModelCombo)->SetChangePosX(pos.x);
            }
            if (ImGui::SliderFloat("Y   ", &pos.y, -100.0f, 100.0f))
            {
                render->GetDrawable(currModelCombo)->SetChangePosY(pos.y);
            }
            if (ImGui::SliderFloat("Z   ", &pos.z, -100.0f, 100.0f))
            {
                render->GetDrawable(currModelCombo)->SetChangePosZ(pos.z);
            }

            ImGui::SeparatorText("Set script");
            ImGui::InputText("Path", text, 200);
            if (ImGui::Button("Add script"))
            {
                render->GetDrawable(currModelCombo)->CreateScript(new Script(text));
                text[0] = '\0';
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

            float position[3] = { 
                (*camera)[focusedCamera]->camDesc.eye.vector4_f32[0],
                (*camera)[focusedCamera]->camDesc.eye.vector4_f32[1],
                (*camera)[focusedCamera]->camDesc.eye.vector4_f32[2] 
            };

            ImGui::InputFloat3("Position", position);
            (*camera)[focusedCamera]->SetPosition(position[0], position[1], position[2]);

            float rotation[2] = { 
                (*camera)[focusedCamera]->camYaw, 
                (*camera)[focusedCamera]->camPitch 
            };

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

    bool UI::SetLightMananger(LightMananger* lman)
    {
        UI::lman = lman;
        return true;
    }

    bool UI::Light()
    {
        ImGui::SeparatorText("Focused light");

        if (!lman->IsHaveLights())
        {
            ImGui::Text("Project doesn't have lights!");
            return false;
        }

        for (; indexLight < lman->GetCurrentCount(); indexLight++)
        {
            namesLights +=  BJEUtils::ParseLightTypeToStr(lman->GetDesc(indexLight).lightType)
                + " " + std::to_string(indexLight) + '\0';
        }
        ImGui::Combo("Light Focus", &currItemComboLights, namesLights.c_str(), IM_ARRAYSIZE(namesLights.c_str()));

        focusedLight = currItemComboLights;

        ImGui::Separator();

        ImGui::Checkbox("Enabled", (bool*)(&lman->lights[focusedLight].enabled));
        ImGui::Checkbox("Enable PBR lights", (bool*)&lman->pbrEnable);
        ImGui::Checkbox("Enable Image-Based Rendering (only irradiance)", (bool*)&lman->ibrEnable);

        float* variables[3] = { &lman->lights[focusedLight].pos.x, &lman->lights[focusedLight].pos.y, &lman->lights[focusedLight].pos.z };
        ImGui::SeparatorText("Light Position");

        ImGui::SliderFloat3("Pos", *variables, -1000.0f,1000.0f, "%.2f");

        if (lman->lights[focusedLight].lightType == BJEUtils::SPOTLIGHT)
        {
            float* variables2[3] = { &lman->lights[focusedLight].dir.x, &lman->lights[focusedLight].dir.y, &lman->lights[focusedLight].dir.z };
            ImGui::SeparatorText("Light Direction");
            ImGui::SliderFloat3("Dir", *variables2, -1.0f, 1.0f, "%.4f");
        }

        float* variables3[3] = { &lman->lights[focusedLight].color.x, &lman->lights[focusedLight].color.y, &lman->lights[focusedLight].color.z };
        ImGui::SeparatorText("Light Color");
        ImGui::ColorEdit3("Color", *variables3);
        
        ImGui::SeparatorText("Light Attenuation");
        ImGui::SliderFloat("Constant" , &lman->lights[focusedLight].att.x, 0.0f, 10.0f);
        ImGui::SliderFloat("Lineral"  , &lman->lights[focusedLight].att.y, 0.0f, 10.0f);
        ImGui::SliderFloat("Quadratic", &lman->lights[focusedLight].att.z, 0.0f, 10.0f);


        return true;
    }

    bool UI::PostProcessing()
    {
        ImGui::SeparatorText("PBR");

        
        
        ImGui::SeparatorText("Lights correction");

        ImGui::SliderFloat("Gamma", &render->mainDesc.gamma, 0.5f, 4.0f);
        ImGui::SliderFloat("Expourse", &render->mainDesc.expourse, 0.1f, 5.0f);


        ImGui::SeparatorText("Post processing");

        ImGui::Checkbox("Inversion", &postProcessingBools[BJEUtils::INVERSION]);

        ImGui::Checkbox("Grey color", &postProcessingBools[BJEUtils::GREY]);
        
        ImGui::Checkbox("Sharpness", &postProcessingBools[BJEUtils::SHARPNESS]);

        ImGui::Checkbox("Simple blur", &postProcessingBools[BJEUtils::SIMPLE_BLUR]);
       
        ImGui::Checkbox("Boundary delineation", &postProcessingBools[BJEUtils::BOUNDARY_DELINEATION]);

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