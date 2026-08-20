#ifndef SHARE_MEMORY_H__
#define SHARE_MEMORY_H__

#if defined(WIN32) || defined(_WIN32_WCE)
#    include <windows.h>
#elif defined(__LINUX__) || defined(ANDROID)
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <sys/mman.h>
#else
#    include <sys/types.h>
#    include <sys/ipc.h>
#    include <sys/shm.h>
#endif

namespace comm
{

#if defined(WIN32) || defined(_WIN32_WCE)

//Windows : Share Memory Template
template<typename KeyT> class CShareMem_WinX
{
public:
	CShareMem_WinX()
	{
		m_hMapping = NULL;
		m_pAddress = NULL;
		m_dwAccess = 0;
	}

	~CShareMem_WinX()
	{
		Delete( );
	}

	inline int Create(KeyT key, size_t length)
	{
		//检查参数
		if( NULL!=m_pAddress || NULL!=m_hMapping ) return -1;

		//调整长度为页的整数倍 ( 4096 * N )
		CONST ULONGLONG AlignToPage = 0x00000000000003FF;
		ULONGLONG length64 = static_cast< ULONGLONG >( length );
		length64 = ( ( length64 + AlignToPage ) & ( ~ AlignToPage ) );

		//创建共享内存，在 64 位中大小可以超过 4 GB 
		HANDLE hFile = static_cast< HANDLE >( INVALID_HANDLE_VALUE );
		DWORD dwSizeLow = static_cast< DWORD >( length64 & 0xFFFFFFFF );
		DWORD dwSizeHigh = static_cast< DWORD >( ( length64 >> 32 ) & 0xFFFFFFFF ); 
		m_hMapping = CreateFileMappingW(hFile, NULL, PAGE_READWRITE, dwSizeHigh, dwSizeLow, key );
		if( m_hMapping!=NULL && GetLastError()==ERROR_ALREADY_EXISTS )
		{
			//已经存在
			CloseHandle(m_hMapping);
			m_hMapping = NULL;
			return -2;
		}
		else if(NULL == m_hMapping)
		{
			//创建失败
			return -3;
		}

		//映射内存
		m_pAddress = MapViewOfFile(m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, length);
		if( NULL != m_pAddress )
		{ 
			m_dwAccess = INT_MAX;
			return 0; 
		}

		CloseHandle(m_hMapping);
		m_hMapping = NULL;
		return -4;
	}

	inline int Open(KeyT key, size_t length, bool write = false)
	{
		//检查参数
		if( NULL!=m_pAddress || NULL!=m_hMapping ) return -1;

		//调整长度为页的整数倍 ( 4096 * N )
		CONST ULONGLONG AlignToPage = 0x00000000000003FF;
		ULONGLONG length64 = static_cast< ULONGLONG >( length );
		length64 = ( ( length64 + AlignToPage ) & ( ~ AlignToPage ) );

		//打开共享内存
		HANDLE hFile = static_cast< HANDLE >( INVALID_HANDLE_VALUE );
		DWORD dwSizeLow = static_cast< DWORD >( length64 & 0xFFFFFFFF );
		DWORD dwSizeHigh = static_cast< DWORD >( ( length64 >> 32 ) & 0xFFFFFFFF ); 
		m_hMapping = CreateFileMappingW(hFile, NULL, PAGE_READWRITE, dwSizeHigh, dwSizeLow, key );
		if(m_hMapping==NULL)
		{
			//创建失败
			return -2;
		}		

		//映射内存
		m_dwAccess = write ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;
		m_pAddress = MapViewOfFile(m_hMapping, m_dwAccess, 0, 0, length);
		if( NULL != m_pAddress )
		{ 
			return 0; 
		}

		CloseHandle(m_hMapping);
		m_hMapping = NULL;
		return -4;
	}

	inline int Delete( )
	{
		//检查参数
		if( NULL==m_pAddress || NULL==m_hMapping ) return -1;

		//删除共享
		UnmapViewOfFile( m_pAddress );
		CloseHandle( m_hMapping );
		m_pAddress = NULL;
		m_hMapping = 0;
		m_dwAccess = 0;
		return 0;
	}

	inline int Close()
	{
		return Delete();
	}

	inline void* Address()
	{
		return m_pAddress;
	}

public:
	static int Exists( KeyT Key )
	{
		CShareMem_WinX< KeyT > ShareMem;
		return ShareMem.Open( Key );
	}

	static int Remove( KeyT Key )
	{
		return 0;
	}

protected:
	HANDLE    m_hMapping;
	LPVOID    m_pAddress;
	DWORD    m_dwAccess;
};

//Define share mem key type & template name
#define SHARE_MEM_KEY_TYPE const wchar_t*
#define SHARE_MEM_TEMPLATE CShareMem_WinX
#define SHARE_MEM_KEY(name , key_1 , key_2) SHARE_MEM_KEY_TYPE name = TEXT(key_2);

#elif defined(__LINUX__) || defined(ANDROID)

//mmap() : Share Memory Template
template<typename KeyT> class CShareMem_MMap
{
public:
	CShareMem_MMap()
	{
		m_pAddress = NULL;
		m_length = 0;
	}

	~CShareMem_MMap()
	{
		Delete();
	}

public:
	inline int Create(KeyT key, size_t length )
	{
		//Check parameter
		if( NULL!=m_pAddress ) return -1;

		int fd = open(key, O_CREAT|O_RDWR|O_TRUNC);
		if (fd < 0){ return -2; }

		m_length = length;

		//下面语句必需,
		//目的是把空的文件变大到与需要映射的内存大小一致。
		lseek(fd, m_length-1, SEEK_SET);      
		write(fd,"",1);

		m_pAddress = (char*)mmap(NULL, m_length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		close(fd);  //这里close不影响已经映射的内存空间
		if (m_pAddress == MAP_FAILED)
		{
			m_pAddress = NULL;
			m_length = 0;
			return -3;
		}

		m_length = length;

		return 0;
	}

	inline int Delete()
	{   
		if (NULL != m_pAddress)
		{
			munmap(m_pAddress, m_length);
			m_pAddress = NULL;
			m_length = 0;
		}

		//return 0:true -1:false
		//return remove(m_key.c_str());     //删除文件可能会影响共享内存不在同一个物理内存块
		return 0;
	}

	inline int Open(KeyT key, size_t length, bool write = false)
	{
		//Check parameter
		if( NULL!=m_pAddress ) return -1;

		int fd = open(key, O_RDWR);
		if (fd < 0){ return -2; }

		int mmap_flags = PROT_READ;
		if (write)
			mmap_flags = mmap_flags|PROT_WRITE;

		size_t filesize = length;
		struct stat statbuff;
		if (stat(key, &statbuff) >= 0)
		{
			filesize = statbuff.st_size;    //获取文件大小
		}

		m_length = (length<filesize) ? length : filesize;
		//m_length = length;

		m_pAddress = (char*)mmap(NULL, m_length, mmap_flags, MAP_SHARED, fd, 0);
		close(fd);
		if (m_pAddress == MAP_FAILED)
		{
			m_pAddress = NULL;
			m_length = 0;
			return -3;
		}

		return 0;
	}

	inline int Close()
	{
		return Delete();
	}

	inline void * Address()
	{
		return (void *)m_pAddress;
	}

	inline int Length()
	{
		return m_length;
	}

public:
	static int Exists( KeyT key )
	{
		int fd = open(key, O_RDONLY);
		if (fd >= 0)
			close(fd);

		return (fd>=0)?0:-2;
	}

	static int Remove( KeyT key )
	{
		return 0;
	}

protected:
	char*           m_pAddress;
	int             m_length;
};

//Define share mem key type & template name
#define SHARE_MEM_KEY_TYPE const char*
#define SHARE_MEM_TEMPLATE CShareMem_MMap

#else

//System V : Share Memory Template
template<typename KeyT> class CShareMem_SysV
{
public:
	CShareMem_SysV( )
	{
		m_pAddress = NULL;
		m_hMapping = -1;
		m_nAccess = 0;
	}

	~CShareMem_SysV( )
	{
		Delete( );
	}

public:
	inline int Create(KeyT key, size_t length )
	{
		//Check parameter
		if( NULL!=m_pAddress || -1!=m_hMapping ) return -1;

		//Get share memory id
		int iFlags = SHM_R|SHM_W|IPC_CREAT|IPC_EXCL;
		m_hMapping = shmget(key, length , iFlags);
		if( -1 == m_hMapping ){ return -2; }

		//Attach share memory
		m_pAddress = shmat(m_hMapping, 0, 0);
		if( NULL != m_pAddress )
		{
			m_nAccess = -1;
			return 0;
		}

		//Remove share memory id
		int hShare = m_hMapping; m_hMapping = -1;
		return shmctl(hShare, IPC_RMID, 0 );
	}

	inline int Delete( )
	{
		//Check parameter
		if( NULL==m_pAddress || -1==m_hMapping ) return -1;
		if( -1 != m_nAccess ) { return Close(); }

		//Detach and remove share memory
		shmdt( m_pAddress ) ; m_pAddress = NULL;
		shmctl( m_hMapping, IPC_RMID, 0 );
		m_hMapping = -1; m_nAccess = 0;
		return 0;
	}

	inline int Open(KeyT key , size_t length, bool write = false)
	{
		//Check parameter
		if( NULL!=m_pAddress || -1!=m_hMapping ) return -1;

		//Open share memory
		m_nAccess = write ? SHM_R : SHM_R|SHM_W ;
		m_hMapping = shmget(key, 0, m_nAccess );
		if( -1 == m_hMapping ) { return -2; }

		// Attach share memory
		m_pAddress = (char*)shmat(m_hMapping, 0, 0);
		return m_pAddress ? 0 : -3 ;
	}

	inline int Close( )
	{
		//Check Parameter
		if(NULL == m_pAddress || -1 == m_hMapping ) return -1;
		if( -1 == m_nAccess ) { return Delete(); }

		//Share memory detech
		void * pAddr = m_pAddress; m_pAddress = NULL;
		m_hMapping = -1; m_nAccess = 0;
		return shmdt( pAddr );
	}

	inline void * Address()
	{
		return (void *)m_pAddress;
	}

public:
	static int Exists(KeyT key)
	{
		return shmget(key, 0, SHM_R);
	}

	static int Remove(KeyT key )
	{
		int hShare = shmget(key, 0, SHM_R|SHM_W );
		if( -1 == hShare ){ return 0; }

		return shmctl(hShare, IPC_RMID , 0 );
	}

protected:
	int        m_nAccess;
	int        m_hMapping;
	char *     m_pAddress;
};

//Define share mem key type & template name
#define SHARE_MEM_KEY_TYPE key_t
#define SHARE_MEM_TEMPLATE CShareMem_SysV
#define SHARE_MEM_KEY(name, key_1, key_2) enum { name = key_1 };
#endif

//Instance Template Class : ShareMemory
typedef SHARE_MEM_TEMPLATE< SHARE_MEM_KEY_TYPE > ShareMemory;

}

#endif //SHARE_MEMORY_H__
