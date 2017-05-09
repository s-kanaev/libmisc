Summary: Miscellaneous library
Name: libmisc
Version: 0.0.1
Release: 1
License: WTF
Group: Libraries
Source0:    libmisc-%{version}.tar.gz
#BuildRequires: pkgconfig(alsa)
#BuildRequires: pkgconfig(vconf)
#BuildRequires: pkgconfig(iniparser)
#BuildRequires: pkgconfig(dlog)
#Provides: libtizen-audio.so

%description
The library contains epoll-base IO service, coroutines, containers, etc.

%prep
#mkdir build
#cd build
#cmake ..
#cd ..
%setup -q -n %{name}-%{version}


%build
#mkdir build
#cd build
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

%files
%defattr(-,root,root,-)
#
#%doc README COPYING ChangeLog
/usr/lib
/usr/include


#/usr/bin/eject

#/usr/man/man1/eject.1
