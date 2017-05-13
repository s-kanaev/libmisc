%define _libdir         /usr/lib
%define _includedir     /usr/include
%define _pkgconfdir     /usr/share/pkgconfig
%define _libname        libmisc

Summary:        Miscellaneous library
Name:           libmisc
Version:        0.0.1
Release:        1
License:        WTF
Group:          Libraries
Source0:        libmisc-%{version}.tar.gz
BuildRequires:  cmake >= 2.8
BuildRequires:  gcc >= 4
Prereq:         /sbin/ldconfig
Provides:       libmisc

%description
The library contains epoll-based IO service, coroutines, containers, etc.

%package devel
Requires:       libmisc
Summary:        libmisc's development files
Group:          Libraries
Provides:       libmisc-devel

%description devel
Header files for use with libmisc's libraries

%prep
%setup -q -n %{name}-%{version}

%build
%cmake
make %{?jobs:-j%jobs}

%install
if [ -e $RPM_BUILD_ROOT ];
then
  rm -Rf $RPM_BUILD_ROOT
fi
cd build
make install DESTDIR=%{buildroot}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%post devel
/sbin/ldconfig

%postun devel
/sbin/ldconfig


%files
%defattr(-,root,root)
%dir %{_libdir}/%{_libname}
%{_libdir}/%{_libname}/lib*.so.*

%files devel
%defattr(-,root,root)
%dir %{_includedir}/%{_libname}
%{_includedir}/%{_libname}/*.h
%{_pkgconfdir}/libmisc.pc
%{_libdir}/%{_libname}/*.so
