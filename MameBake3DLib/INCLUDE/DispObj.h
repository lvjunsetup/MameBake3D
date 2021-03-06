#ifndef DISPOBJH
#define DISPOBJH

//// Direct3D10 includes
//#include <d3dcommon.h>
//#include <dxgi.h>
//#include <d3d10_1.h>
//#include <d3d10.h>
//#include <d3dcompiler.h>
//#include <d3dx10.h>

#include <ChaVecCalc.h>

class CMQOMaterial;
class CPolyMesh3;
class CPolyMesh4;
class CExtLine;

class CDispObj
{
public:

/**
 * @fn
 * CDispObj
 * @breaf コンストラクタ。
 * @return なし。
 */
	CDispObj();

/**
 * @fn
 * ~CDispObj
 * @breaf デストラクタ。
 * @return なし。
 */
	~CDispObj();

/**
 * @fn
 * CreateDispObj
 * @breaf メタセコイアからのデータ用の表示用オブジェクトを作成する。
 * @param (ID3D10Device* pdev) IN Direct3Dのデバイス。
 * @param (CPolyMesh3* pm3) IN 形状データへのポインタ。
 * @param (int hasbone) IN ボーン変形有の場合は１、無しの場合は０。MameBake3Dではメタセコイアデータはボーン無し。
 * @return 成功したら０。
 */
	int CreateDispObj( ID3D10Device* pdev, CPolyMesh3* pm3, int hasbone );


/**
 * @fn
 * CreateDispObj
 * @breaf FBXからのデータ用の表示用オブジェクトを作成する。
 * @param (ID3D10Device* pdev) IN Direct3Dのデバイス。
 * @param (CPolyMesh4* pm4) IN 形状データへのポインタ。
 * @param (int hasbone) IN ボーン変形有の場合は１、無しの場合は０。 * @return 戻り値の説明
 * @return 成功したら０。
 */
	int CreateDispObj( ID3D10Device* pdev, CPolyMesh4* pm4, int hasbone );

/**
 * @fn
 * CreateDispObj
 * @breaf 線分データ用の表示用オブジェクトを作成する。
 * @param (ID3D10Device* pdev) IN Direct3Dのデバイス。
 * @param (CExtLine* extline) IN 線の形状データへのポインタ。
 * @return 成功したら０。
 */
	int CreateDispObj( ID3D10Device* pdev, CExtLine* extline );


/**
 * @fn
 * RenderNormal
 * @breaf 通常のベーシックな方法で３D描画をする。
 * @param (CMQOMaterial* rmaterial) IN 材質へのポインタ。
 * @param (int lightflag) IN 照光処理するときは１、しないときは０。
 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
 * @return 成功したら０。
 */
	int RenderNormal( CMQOMaterial* rmaterial, int lightflag, ChaVector4 diffusemult );

/**
 * @fn
 * RenderNormalPM3
 * @breaf 通常のベーシックな方法でメタセコイアデータを３D表示する。
 * @param (int lightflag) IN 照光処理するときは１、しないときは０。
 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
 * @return 成功したら０。
 * @detail FBXデータは１オブジェクトにつき１マテリアル(材質)だが、メタセコイアデータは１オブジェクトに複数マテリアルが設定されていることが多い。
 */
	int RenderNormalPM3( int lightflag, ChaVector4 diffusemult );

/**
 * @fn
 * RenderLine
 * @breaf 線分データを表示する。
 * @param (ChaVector4 diffusemult) IN ディフューズ色に掛け算する比率。
 * @return 成功したら０。
 */
	int RenderLine( ChaVector4 diffusemult );

/**
 * @fn
 * CopyDispV
 * @breaf FBXの頂点データを表示バッファにコピーする。
 * @param (CPolyMesh4* pm4) IN FBXの形状データ。
 * @return 成功したら０。
 */
	int CopyDispV( CPolyMesh4* pm4 );

/**
 * @fn
 * CopyDispV
 * @breaf メタセコイアの頂点データを表示バッファにコピーする。
 * @param (CPolyMesh3* pm3) IN メタセコイアの形状データ。
 * @param (引数名) 引数の説明
 * @return 成功したら０。
 */
	int CopyDispV( CPolyMesh3* pm3 );


	void SetScale(ChaVector3 srcscale, ChaVector3 srcoffset)
	{
		m_scale = srcscale;
		m_scaleoffset = srcoffset;
	};

private:

/**
 * @fn
 * InitParams
 * @breaf メンバを初期化する。
 * @return ０。
 */
	int InitParams();

/**
 * @fn
 * DestroyObjs
 * @breaf アロケートしたメモリを開放する。
 * @return ０。
 */
	int DestroyObjs();

/**
 * @fn
 * CreateDecl
 * @breaf 表示用頂点データのフォーマット(宣言)を作成する。
 * @return 成功したら０。
 */
	int CreateDecl();

/**
 * @fn
 * CreateVBandIB
 * @breaf ３D表示用の頂点バッファとインデックスバッファを作成する。
 * @return 成功したら０。
 */
	int CreateVBandIB();

/**
 * @fn
 * CreateVBandIBLine
 * @breaf 線分用の頂点バッファとインデックスバッファを作成する。
 * @return 成功したら０。
 */
	int CreateVBandIBLine();

private:
	int m_hasbone;//ボーン変形用のオブジェクトであるとき１、それ以外の時は０。

	ID3D10Device* m_pdev;//外部メモリ、Direct3Dのデバイス。
	CPolyMesh3* m_pm3;//外部メモリ、メタセコイアファイルから作成した３Dデータ。
	CPolyMesh4* m_pm4;//外部メモリ、FBXファイルから作成した３Dデータ。
	CExtLine* m_extline;//外部メモリ、線データ。


	/*
	extern ID3D10EffectTechnique* g_hRenderBoneL0;
	extern ID3D10EffectTechnique* g_hRenderBoneL1;
	extern ID3D10EffectTechnique* g_hRenderBoneL2;
	extern ID3D10EffectTechnique* g_hRenderBoneL3;
	extern ID3D10EffectTechnique* g_hRenderNoBoneL0;
	extern ID3D10EffectTechnique* g_hRenderNoBoneL1;
	extern ID3D10EffectTechnique* g_hRenderNoBoneL2;
	extern ID3D10EffectTechnique* g_hRenderNoBoneL3;
	extern ID3D10EffectTechnique* g_hRenderLine;
	*/


	D3D10_BUFFER_DESC m_BufferDescBone;
	D3D10_BUFFER_DESC m_BufferDescNoBone;
	D3D10_BUFFER_DESC m_BufferDescInf;
	D3D10_BUFFER_DESC m_BufferDescLine;

	ID3D10InputLayout* m_layoutBoneL0;
	ID3D10InputLayout* m_layoutBoneL1;
	ID3D10InputLayout* m_layoutBoneL2;
	ID3D10InputLayout* m_layoutBoneL3;
	ID3D10InputLayout* m_layoutNoBoneL0;
	ID3D10InputLayout* m_layoutNoBoneL1;
	ID3D10InputLayout* m_layoutNoBoneL2;
	ID3D10InputLayout* m_layoutNoBoneL3;
	ID3D10InputLayout* m_layoutLine;

    ID3D10Buffer* m_VB;//表示用頂点バッファ。
	ID3D10Buffer* m_InfB;//表示用ボーン影響度バッファ。
	ID3D10Buffer* m_IB;//表示用三角のインデックスバッファ。


	ChaVector3 m_scale;
	ChaVector3 m_scaleoffset;

};



#endif