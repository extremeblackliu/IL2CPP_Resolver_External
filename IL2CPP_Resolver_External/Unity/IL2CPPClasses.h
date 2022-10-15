#pragma once

struct il2cppImage
{
	const char* m_pName;
	const char* m_pNameNoExt;
	char m_pad0[0x7];
	int m_iClassStartOffset;
	int m_iClassCount;
};

struct il2cppAssemblyName
{
	const char* m_pName;
	const char* m_pCulture;
	const char* m_pHash;
	const char* m_pPublicKey;
	unsigned int m_uHash;
	int m_iHashLength;
	unsigned int m_uFlags;
	int m_iMajor;
	int m_iMinor;
	int m_iBuild;
	int m_bRevision;
	unsigned char m_uPublicKeyToken[8];
};

struct il2cppAssembly
{
	il2cppImage* m_pImage;
	unsigned int m_uToken;
	int m_ReferencedAssemblyStart;
	int m_ReferencedAssemblyCount;
	il2cppAssemblyName m_aName;
};

struct il2cppClass
{
	void* m_pImage;
	void* m_pGC;
	const char* m_pName;
	const char* m_pNamespace;
	void* m_pValue;					//class
	void* m_pArgs;
	il2cppClass* m_pElementClass;
	il2cppClass* m_pCastClass;
	il2cppClass* m_pDeclareClass;
	il2cppClass* m_pParentClass;
	void* m_pGenericClass;
	void* m_pTypeDefinition;
	void* m_pInteropData;
	void* m_pFields;
	void* m_pEvents;
	void* m_pProperties;
	void** m_pMethods;
	il2cppClass** m_pNestedTypes;
	il2cppClass** m_ImplementedInterfaces;
	void* m_pInterfaceOffsets;
	void* m_pStaticFields;
	void* m_pRGCTX;
};

struct il2cppObject
{
	il2cppClass* m_pClass = nullptr;
	void* m_pMonitor = nullptr;
};