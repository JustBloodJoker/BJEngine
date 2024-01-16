cbuffer WorldMatrixBuffer : register(b0)
{
    matrix WVP;
};

struct VS_INPUT
{
    float4 pos : POSITION; 
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(input.pos, WVP);
   
    return output;
}