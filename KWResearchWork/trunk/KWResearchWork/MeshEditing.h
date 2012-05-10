#pragma once

class CKWResearchWorkDoc;

class CMeshEditing
{
public:
	CMeshEditing(void);
	~CMeshEditing(void);

	void Init(CKWResearchWorkDoc* pDataIn);

	void Input2DProfilePoint(CPoint ProfilePoint);
	void Convert2DProfileTo3D();

	void Render(GLenum mode);

private:
	CKWResearchWorkDoc* pDoc;


	vector<CPoint> UserInput2DProfile;
	void Render2DProfile();


};
