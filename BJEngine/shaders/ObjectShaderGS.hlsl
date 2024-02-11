
#define MAX_LIGHT_NUM 5

cbuffer GSBuffer : register(b1)
{
   float time;
   int explode;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION0;
    float4 eyePos : POSITION1;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

   float4 lightViewPosition[MAX_LIGHT_NUM] : TEXCOORD1;
};


float3 genDirection(VS_OUTPUT inp[3])
{
   float3 a = inp[0].worldPos.xyz - inp[1].worldPos.xyz;
   float3 b = inp[2].worldPos.xyz - inp[1].worldPos.xyz;
   return normalize(cross(a, b));
}

float4 explodeMatrixChange(float4 pos, float3 dir)
{
    
}

///////////////// DO NOTHING

[maxvertexcount(3)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream<VS_OUTPUT> OutputStream)
{
    for(int i = 0; i < 3; i++)
    {    
        OutputStream.Append(input[i]);
    }
}
