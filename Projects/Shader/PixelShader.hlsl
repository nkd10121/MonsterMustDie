// サンプラー（DxLibから設定されるテクスチャ）
SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> noizeTex : register(t1);

// ピクセルシェーダーの入力情報
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float3 Normal : Normal;
    float3 Tangent : TANGENT; //接線ベクトル
    float3 Binormal : BINORMAL; //従法線ベクトル
    float2 TexCoords0 : TEXCOORD0; // テクスチャ座標
    float4 svPosition : SV_POSITION; // 座標( プロジェクション空間 )
    float4 Position : POSITION0; // 座標( ワールド空間 )
    float4 P : POSITION1; // 座標( プロジェクション空間 )
    float4 VPosition : POSITION2; // 座標(ワールド空間*ビュー空間)
};

cbuffer UserData : register(b4)
{
    float time; // アニメーション用の時間
	bool isNormalDraw;	//半透明にするかどうか
    float dummy[2];
};

float4 main(PS_INPUT input) : SV_Target0
{
    float2 uv = float2(input.P.x / input.P.z, input.P.y / input.P.z) / 4 + float2(sin(time) * 0.1f, -time * 0.5f);
    float noizeValue = noizeTex.Sample(smp, uv).r + noizeTex.Sample(smp, uv).g + noizeTex.Sample(smp, uv).b;
    noizeValue = (noizeValue * 2.0f - 1.0f);
    
    //float3 color = tex.Sample(smp, input.TexCoords0).rgb * noizeValue;
    float3 color = tex.Sample(smp, input.TexCoords0).bgr * noizeValue;
    float4 ret;
    ret.rgb = color.rgb;
    ret.a = tex.Sample(smp, input.TexCoords0).a * (isNormalDraw * 0.5f + 0.5f);
    return ret;
}