Summary: PaSoRi and Felica library
Name: libpafe
Version: 0.0.7
Release: 0%{?dist}
License: GPLv2
Group: System Environment/Libraries
Source0: http://homepage3.nifty.com/slokar/pasori/libpafe-0.0.7.tar.gz
URL: http://homepage3.nifty.com/slokar/pasori/libpafe.html

BuildRequires: libusb-devel
BuildRequires: gcc
BuildRequires: make
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
The libpafe library can be used by applications to access Ferica via PaSoRi (RC-S320, RC-S330).

%package devel
Group: Development/Libraries
Summary: PaSoRi and Felica library (development files)
Requires: %{name} = %{version}-%{release}

%description devel
The libpafe library can be used by applications to access Ferica via PaSoRi (RC-S320, RC-S330).

%prep
%setup -q

%build

%configure
make 

%install
rm -rf ${RPM_BUILD_ROOT}

%makeinstall
mkdir -p ${RPM_BUILD_ROOT}%{_libdir}
mkdir -p ${RPM_BUILD_ROOT}%{_includedir}
mkdir -p -m755 ${RPM_BUILD_ROOT}%{_sysconfdir}/udev/rules.d

mv ChangeLog ChangeLog.orig
iconv -f euc-jp -t utf-8 ChangeLog.orig > ChangeLog
mv README README.orig
iconv -f euc-jp -t utf-8 README.orig > README

make install DESTDIR=${RPM_BUILD_ROOT} 
install -m755 -d -p ${RPM_BUILD_ROOT}%{_libdir}
install -m644 debian/libpafe.udev $RPM_BUILD_ROOT%{_sysconfdir}/udev/rules.d/10-pasori.rules

rm -f $RPM_BUILD_ROOT%{_libdir}/*.la
rm -f $RPM_BUILD_ROOT%{_libdir}/*.a

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING README ChangeLog
%{_libdir}/lib*.so.*
%{_libdir}/libpafe.so
%{_bindir}/felica_dump
%{_bindir}/pasori_test
%{_sysconfdir}/udev/rules.d/10-pasori.rules

%files devel
%defattr(-,root,root,-)
%{_includedir}/libpafe/*.h

%changelog
* Thu Jul 28 2011 Ryo Fujita <rfujita@redhat.com> - 0.0.7-0
- Initial build for Fedora 15

