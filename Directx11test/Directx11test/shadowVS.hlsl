cbuffer WorldMatrixBuffer : register(b1)
{
    matrix WVP;
};

float4 VS(float4 pos : POSITION) : SV_POSITION
{
    float4 output; 
    output = mul(pos, WVP);
  
    return  output;
}