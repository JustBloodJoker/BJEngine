cbuffer Params : register(b0)
{
	matrix ViewProjection[6];
};


struct VertexOutput
{
	float4 Position : SV_POSITION;   
};

struct GeometryOutput
{
	float4 Position : SV_POSITION;   
	uint RTIndex : SV_RenderTargetArrayIndex;
};

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