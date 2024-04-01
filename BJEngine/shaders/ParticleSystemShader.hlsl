struct Particle 
{
    float4 pos;
    float4 velocity;
    float4 color;
};

StructuredBuffer<Particle> Particles : register(t0); 
cbuffer WorldMatrixBuffer : register(b1)
{
    matrix WVP;
    matrix world;
};

struct VertexInput
{
 	uint VertexID : SV_VertexID;
};

struct PixelInput
{
	float4 Position : SV_POSITION; 
};

struct PixelOutput
{
    float4 Color : SV_TARGET0;
};

PixelInput VS(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	Particle particle = Particles[input.VertexID];

	float4 worldPosition = particle.pos;
	output.Position = mul(worldPosition, WVP);

	return output;
}

[maxvertexcount(1)] 
void GS( point PixelInput input[1], inout PointStream<PixelInput> stream )
{
	PixelInput pointOut = input[0];
	
	stream.Append(pointOut);
	stream.RestartStrip();
}

PixelOutput PS(PixelInput input)
{
	PixelOutput output = (PixelOutput)0;

	output.Color = Particles[0].color;
	
	return output;
}