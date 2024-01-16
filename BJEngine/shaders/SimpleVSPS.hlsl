cbuffer WorldMatrixBuffer : register(b0)
{
    matrix WVP;
}

float4 VS(float4 pos : POSITION) : SV_POSITION
{
    float4 opos = mul(pos, WVP);
    return opos;
}

float4 PS(float4 pos : POSITION) : SV_TARGET
{
    return float4(1.0f,1.0f,1.0f,1.0f);
}
