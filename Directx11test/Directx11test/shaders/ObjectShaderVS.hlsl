cbuffer WorldMatrixBuffer : register(b0)
{
    float4x4 WVP;
    float4x4 world;
    float4x4 cam;
};

struct VS_INPUT
{
    float4 pos : POSITION; 
    float2 texCoord : TEXCOORD; 
    float3 normal : NORMAL; 
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION0;
    float4 eyePos : POSITION1;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    output.pos = mul(input.pos, WVP);
    output.worldPos = mul(input.pos, world);
    output.eyePos = mul(input.pos, cam);
    output.normal = mul(input.normal, world);
    output.normal = normalize(output.normal);
    output.tangent = mul(input.tangent, world);
    output.texCoord = input.texCoord;

    return output;
}