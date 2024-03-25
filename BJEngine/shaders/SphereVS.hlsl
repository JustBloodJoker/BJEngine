cbuffer WorldMatrixBuffer : register(b1)
{
    matrix VP;
    matrix world;
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

    output.pos = mul(input.pos, world);
    output.pos.w = input.pos.w; 

    return output;
}