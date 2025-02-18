#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float32_t value;
    float32_t darkness;
    float32_t effective;
};
ConstantBuffer<Material> gMaterial : register(b0);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
     //Vignetting
     // 周囲をΘに、中心になるほど明るくなる
    float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    //correctだけで計算すると中心の最大値が0.0625で暗すぎるのでScaleを調整
    float vignette = correct.x * correct.y * gMaterial.value;
    //とりあえず0.8倍してみた
    vignette = saturate(pow(vignette, gMaterial.darkness)) ;
    
    output.color.rgb *= lerp(float32_t3(1.0f, 1.0f, 1.0f), vignette, gMaterial.effective);
    return output;
}