

struct outps
{
    float4 pos : POSITION0;
    float4 outp : POSITION1;
};


float PS(float4 depth : SV_POSITION) : SV_TARGET
{	
    //float depth = inp.outp.z / inp.outp.w;
    //depth = 1.0f - saturate((depth - 0.95f) * 20.0f);

    //float bias = 0.0005 + (inp.pos.z - inp.outp.w / inp.pos.w);
    //return 1 - saturate(length(inp.outp.xyz) / 100.0f + bias);
    depth.r = saturate((depth.r - 0.95f) * 20.0f);
    return depth.r;

    //return depth;
}