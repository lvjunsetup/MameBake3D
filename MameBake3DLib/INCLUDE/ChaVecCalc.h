#ifndef ChaCalcH
#define ChaCalcH

#ifdef CONVD3DX9
#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <d3dcompiler.h>
#include <d3dx10.h>
#endif

#include <coef.h>

class CQuaternion;
class btMatrix3x3;
class btVector3;


//class

class ChaMatrix
{
public:
	ChaMatrix();
	ChaMatrix::ChaMatrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);

#ifdef CONVD3DX9
	ChaMatrix::ChaMatrix(D3DXMATRIX m){
		_11 = m._11;
		_12 = m._12;
		_13 = m._13;
		_14 = m._14;
			
		_21 = m._21;
		_22 = m._22;
		_23 = m._23;
		_24 = m._24;

		_31 = m._31;
		_32 = m._32;
		_33 = m._33;
		_34 = m._34;

		_41 = m._41;
		_42 = m._42;
		_43 = m._43;
		_44 = m._44;
	};
#endif

	~ChaMatrix();

	ChaMatrix ChaMatrix::operator= (ChaMatrix m);
#ifdef CONVD3DX9
	ChaMatrix ChaMatrix::operator= (D3DXMATRIX m);
#endif

	ChaMatrix operator* (float srcw) const;
	ChaMatrix &operator*= (float srcw);
	ChaMatrix operator/ (float srcw) const;
	ChaMatrix &operator/= (float srcw);
	ChaMatrix operator+ (const ChaMatrix &m) const;
	ChaMatrix &operator+= (const ChaMatrix &m);
	ChaMatrix operator- (const ChaMatrix &m) const;
	ChaMatrix &operator-= (const ChaMatrix &m);
	ChaMatrix operator* (const ChaMatrix &m) const;
	ChaMatrix &operator*= (const ChaMatrix &m);
	ChaMatrix operator/ (const ChaMatrix &m) const;
	ChaMatrix &operator/= (const ChaMatrix &m);
	ChaMatrix operator- () const;

	bool ChaMatrix::operator== (const ChaMatrix &m) const {
		bool result;
		result = _11 == m._11 && _12 == m._12 && _13 == m._13 && _14 == m._14 &&
			_21 == m._21 && _22 == m._22 && _23 == m._23 && _24 == m._24 &&
			_31 == m._31 && _32 == m._32 && _33 == m._33 && _34 == m._34 &&
			_41 == m._41 && _42 == m._42 && _43 == m._43 && _44 == m._44;
		return result;
	};
	bool ChaMatrix::operator!= (const ChaMatrix &m) const {
		return !(*this == m);
	};

#ifdef CONVD3DX9
	D3DXMATRIX D3DX();
#endif

public:
	 float _11;
	float _12;
	float _13;
	float _14;

	float _21;
	float _22;
	float _23;
	float _24;

	float _31;
	float _32;
	float _33;
	float _34;

	float _41;
	float _42;
	float _43;
	float _44;

};


class ChaVector2
{
public:

	ChaVector2();
	ChaVector2(float srcx, float srcy);
	~ChaVector2();

	ChaVector2 ChaVector2::operator= (ChaVector2 v);
	ChaVector2 operator* (float srcw) const;
	ChaVector2 &operator*= (float srcw);
	ChaVector2 operator/ (float srcw) const;
	ChaVector2 &operator/= (float srcw);
	ChaVector2 operator+ (const ChaVector2 &v) const;
	ChaVector2 &operator+= (const ChaVector2 &v);
	ChaVector2 operator- (const ChaVector2 &v) const;
	ChaVector2 &operator-= (const ChaVector2 &v);
	ChaVector2 operator* (const ChaVector2 &v) const;
	ChaVector2 &operator*= (const ChaVector2 &v);
	//ChaVector2 operator/ (const ChaVector2 &q) const;
	//ChaVector2 &operator/= (const ChaVector2 &q);
	ChaVector2 operator- () const;

	bool operator== (const ChaVector2 &v) const { return x == v.x && y == v.y; };
	bool operator!= (const ChaVector2 &v) const { return !(*this == v); };

#ifdef CONVD3DX9
	D3DXVECTOR2 D3DX();
#endif

public:
	 float x;
	float y;

};


class ChaVector3
{
public:

	ChaVector3();
	ChaVector3(float srcx, float srcy, float srcz);
	~ChaVector3();

	ChaVector3 ChaVector3::operator= (ChaVector3 v);
	ChaVector3 operator* (float srcw) const;
	ChaVector3 &operator*= (float srcw);
	ChaVector3 operator/ (float srcw) const;
	ChaVector3 &operator/= (float srcw);
	ChaVector3 operator+ (const ChaVector3 &v) const;
	ChaVector3 &operator+= (const ChaVector3 &v);
	ChaVector3 operator- (const ChaVector3 &v) const;
	ChaVector3 &operator-= (const ChaVector3 &v);
	ChaVector3 operator* (const ChaVector3 &v) const;
	ChaVector3 &operator*= (const ChaVector3 &v);
	//ChaVector3 operator/ (const ChaVector3 &q) const;
	//ChaVector3 &operator/= (const ChaVector3 &q);
	ChaVector3 operator- () const;

	bool operator== (const ChaVector3 &v) const { return x == v.x && y == v.y && z == v.z; };
	bool operator!= (const ChaVector3 &v) const { return !(*this == v); };

#ifdef CONVD3DX9
	D3DXVECTOR3 D3DX();
#endif

public:
	 float x;
	float y;
	float z;

};




class ChaVector4
{
public:

	ChaVector4();
	ChaVector4(float srcx, float srcy, float srcz, float srcw);
	~ChaVector4();

	ChaVector4 ChaVector4::operator= (ChaVector4 v);
	ChaVector4 operator* (float srcw) const;
	ChaVector4 &operator*= (float srcw);
	ChaVector4 operator/ (float srcw) const;
	ChaVector4 &operator/= (float srcw);
	ChaVector4 operator+ (const ChaVector4 &v) const;
	ChaVector4 &operator+= (const ChaVector4 &v);
	ChaVector4 operator- (const ChaVector4 &v) const;
	ChaVector4 &operator-= (const ChaVector4 &v);
	ChaVector4 operator* (const ChaVector4 &v) const;
	ChaVector4 &operator*= (const ChaVector4 &v);
	ChaVector4 operator- () const;

	bool operator== (const ChaVector4 &v) const { return x == v.x && y == v.y && z == v.z && w == v.w; };
	bool operator!= (const ChaVector4 &v) const { return !(*this == v); };

#ifdef CONVD3DX9
	D3DXVECTOR4 D3DX();
#endif

public:
	 float x;
	float y;
	float z;
	float w;

};

class CQuaternion
{
public:
	CQuaternion();
	CQuaternion(float srcw, float srcx, float srcy, float srcz);
	~CQuaternion();
	int SetParams(float srcw, float srcx, float srcy, float srcz);
	int SetParams(D3DXQUATERNION srcxq);

	int SetAxisAndRot(ChaVector3 srcaxis, float phai);
	int SetAxisAndRot(ChaVector3 srcaxis, double phai);

	int SetRotation(CQuaternion* axisq, ChaVector3 srcdeg);
	int SetRotation(CQuaternion* axisq, double degx, double degy, double degz);
	//int GetAxisAndRot(ChaVector3* axisvecptr, float* frad);
	//int QuaternionToAxisAngle(ChaVector3* dstaxis, float* dstrad);
	int CalcFBXEul(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul, int isfirstbone);
	int CalcFBXEulZXY(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul, int isfirstbone);
	int IsInit();

	//void MakeFromBtMat3x3(btMatrix3x3 eulmat);
	void MakeFromD3DXMat(ChaMatrix eulmat);

	//CQuaternion operator= (const CQuaternion &q) const;
	CQuaternion CQuaternion::operator= (CQuaternion q);
	CQuaternion operator* (float srcw) const;
	CQuaternion &operator*= (float srcw);
	CQuaternion operator/ (float srcw) const;
	CQuaternion &operator/= (float srcw);
	CQuaternion operator+ (const CQuaternion &q) const;
	CQuaternion &operator+= (const CQuaternion &q);
	CQuaternion operator- (const CQuaternion &q) const;
	CQuaternion &operator-= (const CQuaternion &q);
	CQuaternion operator* (const CQuaternion &q) const;
	CQuaternion &operator*= (const CQuaternion &q);
	//CQuaternion operator/ (const CQuaternion &q) const;
	//CQuaternion &operator/= (const CQuaternion &q);
	CQuaternion operator- () const;
	//CQuaternion inv () const;
	CQuaternion normalize();

	int CQuaternion::inv(CQuaternion* dstq);

	bool CQuaternion::operator== (const CQuaternion &q) const { return w == q.w && x == q.x && y == q.y && z == q.z; }
	bool CQuaternion::operator!= (const CQuaternion &q) const { return !(*this == q); }

	float DotProduct(CQuaternion srcq);
	float CalcRad(CQuaternion srcq);

	CQuaternion Slerp(CQuaternion endq, int framenum, int frameno);
	int Slerp2(CQuaternion endq, double srcrate, CQuaternion* dstq);

	int Squad(CQuaternion q0, CQuaternion q1, CQuaternion q2, CQuaternion q3, float t);

	ChaMatrix MakeRotMatX();

	int RotationArc(ChaVector3 srcvec0, ChaVector3 srcvec1);
	int Rotate(ChaVector3* dstvec, ChaVector3 srcvec);

	int Q2X(D3DXQUATERNION* dstx);
	int Q2X(D3DXQUATERNION* dstx, CQuaternion srcq);

	int transpose(CQuaternion* dstq);

	int CalcSym(CQuaternion* dstq);
	ChaMatrix CalcSymX2();


	//���˂�
	int Q2EulZXY(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);
	int Q2EulYXZ(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);
	int Q2EulXYZ(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);//bullet��XYZ�̏�
	int Q2EulZYX(int needmodifyflag, CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);


	int Q2Eul(CQuaternion* axisq, ChaVector3 befeul, ChaVector3* reteul);
	int Q2EulBt(ChaVector3* reteul);

	void RotationMatrix(ChaMatrix srcmat);

	//inout : srcdstq
	int InOrder(CQuaternion* srcdstq);

private:

	float vecDotVec(ChaVector3* vec1, ChaVector3* vec2);
	float lengthVec(ChaVector3* vec);
	float aCos(float dot);
	int vec3RotateY(ChaVector3* dstvec, float deg, ChaVector3* srcvec);
	int vec3RotateX(ChaVector3* dstvec, float deg, ChaVector3* srcvec);
	int vec3RotateZ(ChaVector3* dstvec, float deg, ChaVector3* srcvec);

	int ModifyEuler(ChaVector3* eulerA, ChaVector3* eulerB);
	int GetRound(float srcval);


public:
	 float x;
	float y;
	float z;
	float w;
};


#ifdef CHACALCCPP
float ChaVector3Length(ChaVector3* psrc);
void ChaVector3Normalize(ChaVector3* pdst, const ChaVector3* psrc);
float ChaVector3Dot(const ChaVector3* psrc1, const ChaVector3* psrc2);
void ChaVector3Cross(ChaVector3* pdst, const ChaVector3* psrc1, const ChaVector3* psrc2);
void ChaVector3TransformCoord(ChaVector3* pdst, ChaVector3* psrc, ChaMatrix* pmat);
float ChaVector3LengthSq(ChaVector3* psrc);
ChaVector3* ChaVector3TransformNormal(ChaVector3 *pOut, const ChaVector3* pV, const ChaMatrix* pM);


void ChaMatrixIdentity(ChaMatrix* pdst);
void ChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc);
void ChaMatrixTranslation(ChaMatrix* pdst, float srcx, float srcy, float srcz);
void ChaMatrixTranspose(ChaMatrix* pdst, ChaMatrix* psrc);
void ChaMatrixRotationAxis(ChaMatrix* pdst, ChaVector3* srcaxis, float srcrad);
void ChaMatrixScaling(ChaMatrix* pdst, float srcx, float srcy, float srcz);
void ChaMatrixLookAtRH(ChaMatrix* dstviewmat, ChaVector3* camEye, ChaVector3* camtar, ChaVector3* camUpVec);
ChaMatrix* ChaMatrixOrthoOffCenterRH(ChaMatrix* pOut, float l, float r, float t, float b, float zn, float zf);
ChaMatrix* ChaMatrixPerspectiveFovRH(ChaMatrix* pOut, float fovY, float Aspect, float zn, float zf);
ChaMatrix* ChaMatrixRotationYawPitchRoll(ChaMatrix* pOut, float srcyaw, float srcpitch, float srcroll);
ChaMatrix* ChaMatrixRotationX(ChaMatrix* pOut, float srcrad);
ChaMatrix* ChaMatrixRotationY(ChaMatrix* pOut, float srcrad);
ChaMatrix* ChaMatrixRotationZ(ChaMatrix* pOut, float srcrad);


const ChaMatrix* ChaMatrixRotationQuaternion(ChaMatrix* dstmat, CQuaternion* srcq);

void CQuaternionIdentity(CQuaternion* dstq);



int qToEulerAxis(CQuaternion axisQ, CQuaternion* srcq, ChaVector3* Euler);
int modifyEuler(ChaVector3* eulerA, ChaVector3* eulerB);// new, old

float vecDotVec(ChaVector3* vec1, ChaVector3* vec2);
float lengthVec(ChaVector3* vec);
float aCos(float dot);
int vec3RotateY(ChaVector3* dstvec, float deg, ChaVector3* srcvec);
int vec3RotateX(ChaVector3* dstvec, float deg, ChaVector3* srcvec);
int vec3RotateZ(ChaVector3* dstvec, float deg, ChaVector3* srcvec);

int GetRound(float srcval);
int IsInitRot(ChaMatrix srcmat);
int IsSameMat(ChaMatrix srcmat1, ChaMatrix srcmat2);
int IsSameEul(ChaVector3 srceul1, ChaVector3 srceul2);

void InitAngleLimit(ANGLELIMIT* dstal);
void SetAngleLimitOff(ANGLELIMIT* dstal);


bool IsTimeEqual(double srctime1, double srctime2);

float VecLength(ChaVector3 srcvec);
void GetSRTMatrix(ChaMatrix srcmat, ChaVector3* svecptr, ChaMatrix* rmatptr, ChaVector3* tvecptr);

ChaMatrix TransZeroMat(ChaMatrix srcmat);
ChaMatrix ChaMatrixFromBtMat3x3(btMatrix3x3* srcmat3x3);
ChaMatrix ChaMatrixFromBtTransform(btMatrix3x3* srcmat3x3, btVector3* srcpipot);
ChaMatrix ChaMatrixInv(ChaMatrix srcmat);


CQuaternion QMakeFromBtMat3x3(btMatrix3x3* eulmat);

#else
extern float ChaVector3Length(ChaVector3* psrc);
extern void ChaVector3Normalize(ChaVector3* pdst, const ChaVector3* psrc);
extern float ChaVector3Dot(const ChaVector3* psrc1, const ChaVector3* psrc2);
extern void ChaVector3Cross(ChaVector3* pdst, const ChaVector3* psrc1, const ChaVector3* psrc2);
extern void ChaVector3TransformCoord(ChaVector3* pdst, ChaVector3* psrc, ChaMatrix* pmat);
extern float ChaVector3LengthSq(ChaVector3* psrc);
extern ChaVector3* ChaVector3TransformNormal(ChaVector3 *pOut, const ChaVector3* pV, const ChaMatrix* pM);

extern void ChaMatrixIdentity(ChaMatrix* pdst);
extern void ChaMatrixInverse(ChaMatrix* pdst, float* pdet, const ChaMatrix* psrc);
extern void ChaMatrixTranslation(ChaMatrix* pdst, float srcx, float srcy, float srcz);
extern void ChaMatrixTranspose(ChaMatrix* pdst, ChaMatrix* psrc);
extern void ChaMatrixRotationAxis(ChaMatrix* pdst, ChaVector3* srcaxis, float srcrad);
extern void ChaMatrixScaling(ChaMatrix* pdst, float srcx, float srcy, float srcz);
extern void ChaMatrixLookAtRH(ChaMatrix* dstviewmat, ChaVector3* camEye, ChaVector3* camtar, ChaVector3* camUpVec);
extern ChaMatrix* ChaMatrixOrthoOffCenterRH(ChaMatrix* pOut, float l, float r, float t, float b, float zn, float zf);
extern ChaMatrix* ChaMatrixPerspectiveFovRH(ChaMatrix* pOut, float fovY, float Aspect, float zn, float zf);
extern ChaMatrix* ChaMatrixRotationYawPitchRoll(ChaMatrix* pOut, float srcyaw, float srcpitch, float srcroll);
extern ChaMatrix* ChaMatrixRotationX(ChaMatrix* pOut, float srcrad);
extern ChaMatrix* ChaMatrixRotationY(ChaMatrix* pOut, float srcrad);
extern ChaMatrix* ChaMatrixRotationZ(ChaMatrix* pOut, float srcrad);

extern const ChaMatrix* ChaMatrixRotationQuaternion(ChaMatrix* dstmat, CQuaternion* srcq);

extern void CQuaternionIdentity(CQuaternion* dstq);


extern int qToEulerAxis(CQuaternion axisQ, CQuaternion* srcq, ChaVector3* Euler);
extern int modifyEuler(ChaVector3* eulerA, ChaVector3* eulerB);// new, old

extern float vecDotVec(ChaVector3* vec1, ChaVector3* vec2);
extern float lengthVec(ChaVector3* vec);
extern float aCos(float dot);
extern int vec3RotateY(ChaVector3* dstvec, float deg, ChaVector3* srcvec);
extern int vec3RotateX(ChaVector3* dstvec, float deg, ChaVector3* srcvec);
extern int vec3RotateZ(ChaVector3* dstvec, float deg, ChaVector3* srcvec);

extern int GetRound(float srcval);
extern int IsInitRot(ChaMatrix srcmat);
extern int IsSameMat(ChaMatrix srcmat1, ChaMatrix srcmat2);
extern int IsSameEul(ChaVector3 srceul1, ChaVector3 srceul2);

extern void InitAngleLimit(ANGLELIMIT* dstal);
extern void SetAngleLimitOff(ANGLELIMIT* dstal);


extern bool IsTimeEqual(double srctime1, double srctime2);

extern float VecLength(ChaVector3 srcvec);
extern void GetSRTMatrix(ChaMatrix srcmat, ChaVector3* svecptr, ChaMatrix* rmatptr, ChaVector3* tvecptr);

extern ChaMatrix TransZeroMat(ChaMatrix srcmat);
extern ChaMatrix ChaMatrixFromBtMat3x3(btMatrix3x3* srcmat3x3);
extern ChaMatrix ChaMatrixFromBtTransform(btMatrix3x3* srcmat3x3, btVector3* srcpipot);
extern ChaMatrix ChaMatrixInv(ChaMatrix srcmat);


extern CQuaternion QMakeFromBtMat3x3(btMatrix3x3* eulmat);

#endif


//struct

typedef  struct tag_rpselem
{
	int framecnt;
	int skelno;
	float confidence;
	ChaVector3 pos;
	int twistflag;
}RPSELEM;


typedef  struct tag_pickinfo
{
	int buttonflag;//WM_LBUTTONDOWN-->PICK_L, WM_RBUTTONDOWN-->PICK_R, WM_MBUTTONDOWN-->PICK_M�B�����Ă��Ȃ��Ƃ�-->PICK_NONE�B
				   //�ȉ��Abuttonflag��PICK_NONE�ȊO�̎��ɈӖ������B
	POINT mousepos;
	POINT mousebefpos;
	POINT clickpos;
	ChaVector2 diffmouse;
	ChaVector2 firstdiff;
	int winx;
	int winy;
	int pickrange;
	int pickobjno;
	ChaVector3 objscreen;
	ChaVector3 objworld;
}PICKINFO;

typedef  struct tag_cpmot
{
	int boneno;
	double frame;
	ChaVector3 eul;
	ChaVector3 tra;
}CPMOT;


typedef  struct tag_texv
{
	ChaVector3 pos;
	ChaVector2 uv;
}TEXV;

typedef struct  tag_spritev {
	ChaVector4 pos;
	ChaVector2 uv;
} SPRITEV;


typedef struct tag_verface
{
	int			faceno;
	int			orgfaceno;
	int			materialno;
	ChaVector3	facenormal;
}PERFACE;

typedef  struct tag_pervert
{
	int				indexno;//3�p�̏���
	int				vno;
	int				uvnum;
	ChaVector2		uv[2];
	int				vcolflag;
	DWORD			vcol;
	ChaVector3 smnormal;

	int				createflag;
	//���_���쐬���Ȃ��ꍇ�O
	//UV, VCOL, Materialno�̈Ⴂ�ɂ��쐬����ꍇ�͂P�𑫂�
	//normal�ɂ��쐬����ꍇ�͂Q�𑫂�

}PERVERT;

typedef  struct tag_pm3optv
{
	int orgvno;
	int orgfaceno;
	ChaVector3 pos;
	int materialno;
	ChaVector3 normal;
	int				uvnum;
	ChaVector2		uv[2];
	int				vcolflag;
	DWORD			vcol;
}PM3OPTV;

typedef  struct tag_pm3dispv
{
	ChaVector4		pos;
	ChaVector3		normal;
	ChaVector2		uv;
}PM3DISPV;

typedef  struct tag_extlinev
{
	ChaVector4 pos;
}EXTLINEV;


typedef  struct tag_modelbound
{
	ChaVector3 min;
	ChaVector3 max;
	ChaVector3 center;
	float		r;
}MODELBOUND;

typedef  struct tag_n3p
{
	PERFACE*	perface;
	PERVERT*	pervert;
	N3SM*		n3sm;
}N3P;//n*3


#endif


