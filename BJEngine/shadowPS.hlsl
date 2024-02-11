cbuffer WorldMatrixBuffer : register(b1)
{
    matrix WVP;
};

struct outps
{
    float4 pos : POSITION0;
    float4 outp : POSITION1;
};


float PS(float4 depth : SV_POSITION) : SV_DEPTH
{	
    return depth.z;
}