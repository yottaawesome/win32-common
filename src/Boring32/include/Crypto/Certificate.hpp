#pragma once
#include <vector>
#include <Windows.h>
#include <wincrypt.h>

namespace Boring32::Crypto
{
	class Certificate
	{
		public:
			virtual ~Certificate();
			Certificate();
			Certificate(const Certificate& other);
			Certificate(Certificate&& other) noexcept;
			Certificate(PCCERT_CONTEXT m_certContext, const bool ownedExclusively);

		public:
			virtual Certificate& operator=(const Certificate& other);
			virtual Certificate& operator=(Certificate&& other) noexcept;
			virtual bool operator==(PCCERT_CONTEXT const other) const noexcept;
			virtual operator bool() const noexcept;

		public:
			virtual void Close() noexcept;
			virtual PCCERT_CONTEXT GetCert() const noexcept;
			virtual std::wstring GetFormattedSubject(const DWORD format) const;
			virtual std::wstring GetFormattedIssuer(const DWORD format) const;
			virtual std::vector<std::byte> GetIssuer() const;
			virtual std::vector<std::byte> GetSubject() const;
			virtual std::wstring GetSignature() const;
			virtual std::wstring GetSignatureHashCngAlgorithm() const;
			virtual void Attach(PCCERT_CONTEXT attachTo);
			virtual PCCERT_CONTEXT Detach() noexcept;

		public:
			//virtual std::wstring _GetSubjectName() const;

		protected:
			virtual Certificate& Copy(const Certificate& other);
			virtual Certificate& Move(Certificate& other) noexcept;
			virtual std::vector<std::byte> InternalCertGetProperty(
				const DWORD property
			) const;

		protected:
			PCCERT_CONTEXT m_certContext;
	};
}
