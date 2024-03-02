struct VertexOutput
{
	float4 Position : SV_POSITION;   
};

cbuffer Params : register(b0)
{
	matrix ViewProjection;
};

cbuffer WorldMatrixBuffer : register(b1)
{
    matrix World;
};

VertexOutput VS(float4 pos : POSITION)
{
    VertexOutput output; 
    
    output.Position = mul(pos, World);
    output.Position = mul(output.Position, ViewProjection);

    return  output;
}

float PS(float4 depth : SV_POSITION) : SV_DEPTH
{	
    return depth.z;
}

