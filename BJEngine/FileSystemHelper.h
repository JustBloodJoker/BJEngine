#pragma once



#define PROJECT_FILE_START "BJENGINE PROJECT"
#define PROJECT_FILE_VERSION "VERSION" 
#define PROJECT_FILE_END   "END"

#define PROJECT_MATERIALS_START          "MATERIALS"
#define PROJECT_MODELS_DEFFERED_RENDER   "DEFFERED_RENDER_MODELS"
#define PROJECT_MODELS_FORWARD_RENDER    "FORWARD_RENDER_MODELS"
#define PROJECT_LIGHTS            "LIGHT_BUFFER"
#define PROJECT_SEPARATOR                "-------------------------------------"

#define PROJECT_MATERIAL_NAME   "material_name"
#define PROJECT_MATERIAL_END   "material_end"

////////////////////////////////
// MATERIALS

#define PROJECT_MATERIAL_AMBIENT        "ambient"
#define PROJECT_MATERIAL_DIFFUSE        "diffuse"
#define PROJECT_MATERIAL_SPECULAR       "specular"
#define PROJECT_MATERIAL_SPECULAR_POWER "specular_power"
#define PROJECT_MATERIAL_EMISSIVE       "emissive"
#define PROJECT_MATERIAL_OPACITY        "opacity"

#define PROJECT_MATERIAL_TEXTURE_DIFFUSE    "diffuse_texture"
#define PROJECT_MATERIAL_TEXTURE_NORMAL     "normal_texture"
#define PROJECT_MATERIAL_TEXTURE_ROUGHNESS  "roughness_texture"
#define PROJECT_MATERIAL_TEXTURE_EMISSION   "emission_texture"
#define PROJECT_MATERIAL_TEXTURE_SPECULAR   "specular_texture"

////////////////////////////////
// MODELS


#define PROJECT_MODEL_START             "model_param_start"
#define PROJECT_MODEL_VERTICES          "vertices"
#define PROJECT_MODEL_INDICES           "indices"
#define PROJECT_MODEL_MATERIAL          "material"
#define PROJECT_SKYBOX_TEXTURE          "skybox_texture"
#define PROJECT_MODEL_END               "model_param_end"

///////////////////////////////
// LIGHTS

#define PROJECT_LIGHTS_COUNT            "LIGHT_COUNT"
#define PROJECT_LIGHTS_PBR              "PBR"
#define PROJECT_LIGHTS_PARAMS_START     "light_params_start"      
#define PROJECT_LIGHTS_PARAMS_END       "light_params_end"      
