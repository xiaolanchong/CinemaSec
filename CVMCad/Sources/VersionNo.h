#define FILEVER        1,0,0,1
#define PRODUCTVER     1,0,0,1
#define STRFILEVER     "1, 0, 0, 1\0"
#define STRPRODUCTVER  "1, 0, 0, 1\0"



#ifndef VERSION
#define VERSION

#pragma warning(disable : 4505 4710)
#pragma comment(lib, "version.lib")

namespace version_nmsp
{
	struct language
	{
		WORD language_;
		WORD code_page_;

		language()
		{
			language_  = 0;
			code_page_ = 0;
		}
	};
}


class version
{
public:
	version()
	{
		// Get application name
		wchar_t buf[MAX_PATH];

		if(::GetModuleFileName(0, buf, sizeof(buf)))
		{
			std::wstring app_name = buf;
			app_name             = app_name.substr(app_name.rfind(L"\\") + 1);

			// Get version info
			DWORD h = 0;

			DWORD resource_size = ::GetFileVersionInfoSize(const_cast<wchar_t*>(app_name.c_str()), &h);
			if(resource_size)
			{
				resource_data_ = new unsigned char[resource_size];
				if(resource_data_)
				{
					if(::GetFileVersionInfo(const_cast<wchar_t*>(app_name.c_str()),
						0,
						resource_size,
						static_cast<LPVOID>(resource_data_)) != FALSE)
					{
						UINT size = 0;

						// Get language information
						if(::VerQueryValue(static_cast<void*>(resource_data_),
							L"\\VarFileInfo\\Translation",
							reinterpret_cast<LPVOID*>(&language_information_),
							&size) == FALSE)
							throw std::exception("Requested localized version information not available");
					}
					else
					{
						std::stringstream exception;
						exception << L"Could not get version information (Windows error: " << ::GetLastError() << ")";
						throw std::exception(exception.str().c_str());
					}
				}
				else
					throw std::bad_alloc();
			}
			else
			{
				std::stringstream exception;
				exception << L"No version information found (Windows error: " << ::GetLastError() << L")";
				throw std::exception(exception.str().c_str());
			}
		}
		else
			throw std::exception("Could not get application name");
	}

	~version() { delete [] resource_data_; }
	std::wstring get_product_name() const { return get_value(L"ProductName"); }
	std::wstring get_internal_name() const { return get_value(L"InternalName"); }
	std::wstring get_product_version() const { return get_value(L"ProductVersion"); }
	std::wstring get_special_build() const { return get_value(L"SpecialBuild"); }
	std::wstring get_private_build() const { return get_value(L"PrivateBuild"); }
	std::wstring get_copyright() const { return get_value(L"LegalCopyright"); }
	std::wstring get_trademarks() const { return get_value(L"LegalTrademarks"); }
	std::wstring get_comments() const { return get_value(L"Comments"); }
	std::wstring get_company_name() const { return get_value(L"CompanyName"); }
	std::wstring get_file_version() const { return get_value(L"FileVersion"); }
	std::wstring get_file_description() const { return get_value(L"FileDescrition"); }

private:
	unsigned char       *resource_data_;
	version_nmsp::language *language_information_;

	std::wstring get_value(const std::wstring &key) const
	{
		if(resource_data_)
		{
			UINT              size   = 0;
			std::wstringstream t;
			LPVOID            value  = 0;

			// Build query string
			t << L"\\StringFileInfo\\" << std::setw(4) << std::setfill(L'0') << std::hex
				<< language_information_->language_ << std::setw(4) << std::hex
				<< language_information_->code_page_ << L"\\" << key;

			if(::VerQueryValue(static_cast<void*>(resource_data_),
				const_cast<wchar_t*>(t.str().c_str()),
				static_cast<void**>(&value), &size) != FALSE )
			{
				DWORD dwLastError = GetLastError();
				return static_cast<wchar_t*>(value);
			}
		}

		DWORD dwLastError = GetLastError();
		return L"";
	}
};

#endif



