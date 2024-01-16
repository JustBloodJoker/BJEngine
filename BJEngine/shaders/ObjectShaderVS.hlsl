#define MAX_LIGHT_NUM 5

cbuffer WorldMatrixBuffer : register(b1)
{
    matrix WVP;
    matrix world;
    matrix cam;
    matrix projection;
    float4 lightpos;
    matrix lview[MAX_LIGHT_NUM];
    matrix lproj[MAX_LIGHT_NUM];
};

struct VS_INPUT
{
    float4 pos : POSITION; 
    float2 texCoord : TEXCOORD; 
    float3 normal : NORMAL; 
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION0;
    float4 eyePos : POSITION1;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

   float4 lightViewPosition[MAX_LIGHT_NUM] : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(input.pos, WVP);
    output.worldPos = mul(input.pos, world);
    output.eyePos = mul(input.pos, cam);
    output.normal = mul(input.normal, world);
    output.tangent = mul(input.tangent, world);
    output.bitangent = mul(input.bitangent, world);
    output.texCoord = input.texCoord;

    for(int i = 0; i < MAX_LIGHT_NUM; i++)
    {
        output.lightViewPosition[i] = mul(input.pos, world);
        output.lightViewPosition[i] = mul(output.lightViewPosition[i], lview[i]);
        output.lightViewPosition[i] = mul(output.lightViewPosition[i], lproj[i]);
    }
    return output;
}