cbuffer ShadowMapCubeViewProj : register( b1 )
{
    matrix World;
    matrix view[6];
    matrix proj;
};

struct GS_OUTPUT
{
    float4 Pos: SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void PointShadowGenGS(triangle float4 InPos[3] : SV_Position, inout TriangleStream<GS_OUTPUT> OutStream)
{
    for (int iFace = 0; iFace < 6; ++iFace)
    {
        GS_OUTPUT output;

        output.RTIndex = iFace;

        for (int v = 0; v < 3; ++v)
        {
            output.Pos = mul(InPos[v], view[iFace]);
            output.Pos = mul(InPos[v], proj);
            OutStream.Append(output);
        }
        OutStream.RestartStrip();
    }
}