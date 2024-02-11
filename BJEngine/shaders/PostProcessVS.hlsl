cbuffer WorldMatrixBuffer : register(b0)
{
    matrix WVP;
};

struct VS_INPUT
{
    float4 pos : POSITION; 
    float2 texCoord : TEXCOORD; 
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(input.pos, WVP);
    output.texCoord = input.texCoord;

    return output;
}