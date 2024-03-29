cbuffer WorldMatrixBuffer : register(b1)
{
    matrix WVP;
    matrix world;
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
    float3 normal : TEXCOORD1;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texCoord : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    output.pos = mul(input.pos, WVP);
    output.worldPos = mul(input.pos, world);
   
    output.normal = mul(input.normal, world);
    output.tangent = mul(input.tangent, world);
    output.bitangent = mul(input.bitangent, world);
    output.texCoord = input.texCoord;

    return output;
}