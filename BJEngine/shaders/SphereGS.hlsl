#define SPHERE_SLICE_COUNT 6
#define SPHERE_STACK_COUNT 7
#define NUM_SPHERE_VERTICES (SPHERE_SLICE_COUNT * 6 + ((SPHERE_STACK_COUNT - 2) * (SPHERE_SLICE_COUNT + 1)) * 6)
#define PI 3.14159265359
 
struct GSInput {
    float4 PosWS : POSITION;
};
 
cbuffer cbPerFrame : register (b1) {
    matrix ViewProjection;
    matrix QuadHalfSize;
};
 
struct GSOutput {
    float4 PosH : SV_POSITION;
};
 
[maxvertexcount(NUM_SPHERE_VERTICES)]
void GS(point GSInput input[1], inout TriangleStream<GSOutput> triangleStream) 
{
    float SPHERE_RADIUS = input[0].PosWS.w;

    const uint positionsSize = 2 + (SPHERE_STACK_COUNT - 1) * (SPHERE_SLICE_COUNT + 1);
    
    float4 positions[positionsSize] = 
    {
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),
        float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0),float4(0,0,0,0)
    };

  
    

    float4 topVertex = float4(input[0].PosWS.x, input[0].PosWS.y + SPHERE_RADIUS, input[0].PosWS.z, 1.0f);
    float4 bottomVertex = float4(input[0].PosWS.x, input[0].PosWS.y - SPHERE_RADIUS, input[0].PosWS.z, 1.0f);
 
    positions[0] = mul(topVertex, ViewProjection);
 
    const float phiStep = PI / SPHERE_STACK_COUNT;
    const float thetaStep = 2.0f * PI / SPHERE_SLICE_COUNT;
 
    uint currentIndex = 1;
    for (uint i = 1; i < SPHERE_STACK_COUNT; ++i) {
        const float phi = i * phiStep;
        const float sinPhi = sin(phi);
        const float cosPhi = cos(phi);
 
        for (uint j = 0; j <= SPHERE_SLICE_COUNT; ++j) {
            const float theta = j * thetaStep;
 
            float4 v = input[0].PosWS;
 
            v.x += SPHERE_RADIUS * sinPhi * cos(theta);
            v.y += SPHERE_RADIUS * cosPhi;
            v.z += SPHERE_RADIUS * sinPhi * sin(theta);
 
            positions[currentIndex] = mul(v, ViewProjection);
            ++currentIndex;
        }
    }
 
    positions[currentIndex] = mul(bottomVertex, ViewProjection);
 
    GSOutput output;
 
    for (uint k = 1; k <= SPHERE_SLICE_COUNT; ++k) {
        output.PosH = positions[0];
        triangleStream.Append(output);
 
        output.PosH = positions[k + 1];
        triangleStream.Append(output);
 
        output.PosH = positions[k];
        triangleStream.Append(output);
 
        triangleStream.RestartStrip();
    }
 
    uint baseIndex = 1;
    const uint ringVertexCount = SPHERE_SLICE_COUNT + 1;
    for (uint n = 0; n < SPHERE_STACK_COUNT - 2; ++n) {
        for (uint m = 0; m <= SPHERE_SLICE_COUNT; ++m) {
            output.PosH = positions[n * ringVertexCount + m];
            triangleStream.Append(output);
 
            output.PosH = positions[n * ringVertexCount + m + 1];
            triangleStream.Append(output); 
 
            output.PosH = positions[(n + 1) * ringVertexCount + m];
            triangleStream.Append(output);
 
            triangleStream.RestartStrip();
 
            output.PosH = positions[(n + 1) * ringVertexCount + m];
            triangleStream.Append(output);
 
            output.PosH = positions[n * ringVertexCount + m + 1];
            triangleStream.Append(output);
 
            output.PosH = positions[(n + 1) * ringVertexCount + m + 1];
            triangleStream.Append(output);
 
            triangleStream.RestartStrip();
        }
    }
 
    const uint southPoleIndex = positionsSize - 1;
 
    uint offset = southPoleIndex - ringVertexCount;
    for (uint l = 0; l < SPHERE_SLICE_COUNT; ++l) {
        output.PosH = positions[southPoleIndex];
        triangleStream.Append(output);
 
        output.PosH = positions[offset + l];
        triangleStream.Append(output);
 
        output.PosH = positions[offset + l + 1];
        triangleStream.Append(output);
 
        triangleStream.RestartStrip();
    }
}