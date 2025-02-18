// �T���v���[�iDxLib����ݒ肳���e�N�X�`���j
SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> noizeTex : register(t1);

// �s�N�Z���V�F�[�_�[�̓��͏��
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float3 Normal : Normal;
    float3 Tangent : TANGENT; //�ڐ��x�N�g��
    float3 Binormal : BINORMAL; //�]�@���x�N�g��
    float2 TexCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 svPosition : SV_POSITION; // ���W( �v���W�F�N�V������� )
    float4 Position : POSITION0; // ���W( ���[���h��� )
    float4 P : POSITION1; // ���W( �v���W�F�N�V������� )
    float4 VPosition : POSITION2; // ���W(���[���h���*�r���[���)
};

cbuffer UserData : register(b4)
{
    float time; // �A�j���[�V�����p�̎���
	bool isNormalDraw;	//�������ɂ��邩�ǂ���
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