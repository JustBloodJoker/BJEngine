struct VertexOutput
{
	float4 Position : SV_POSITION;
	float4 worldPosition : POSITION;   
};

struct GeometryOutput
{
	float4 Position : SV_POSITION;
	float4 worldPosition : POSITION;      
	uint RTIndex : SV_RenderTargetArrayIndex;
};

cbuffer Params : register(b0)
{
	matrix ViewProjection[6];
};

cbuffer WorldMatrixBuffer : register(b1)
{
    matrix World;
};

VertexOutput VS(float4 pos : POSITION)
{
    VertexOutput output; 
    output.Position = mul(pos, World);
	output.worldPosition = pos;

    return  output;
}

[maxvertexcount(18)]
void GS( triangle VertexOutput input[3], inout TriangleStream<GeometryOutput> CubeMapStream )
{
	[unroll]
    for( int f = 0; f < 6; ++f )
    {
		{
	        GeometryOutput output = (GeometryOutput)0;

			output.RTIndex = f;

			[unroll]
			for( int v = 0; v < 3; ++v )
			{
				float4 worldPosition = input[v].Position;
				output.Position = mul(worldPosition, ViewProjection[f]);

				CubeMapStream.Append( output );
			}

			CubeMapStream.RestartStrip();
        }
    }
}

TextureCube SkyBoxMap : register(t0);
SamplerState SamplerStateWrap : register(s0);
#define PI 3.14159265358979323846

float4 ConventionSkyBoxPS(VertexOutput input) : SV_TARGET0
{	
	float3 normal = normalize(input.worldPosition);
  
    float3 irradiance = float3(0.0f,0.0f,0.0f);
  
	float3 up = float3(0.0, 1.0, 0.0);
	float3 right = cross(up, normal);
	up = cross(normal, right);
 
	float sampleDelta = 0.025;
	float nrSamples = 0.0; 
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
   		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
    	{
        // Перевод сферических координат в декартовы (в касательном пространстве)
        	float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
 
        	// Переход от касательного пространства к мировому
        	float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z*normal; 
 
        	irradiance += SkyBoxMap.Sample( SamplerStateWrap, input.worldPosition ) * cos(theta) * sin(theta);
        	nrSamples++;
    	}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));


    return float4(irradiance, 1.0f);
}
