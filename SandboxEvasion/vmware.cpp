#include "vmware.h"

namespace SandboxEvasion {

VEDetection* VMWare::create_instance(const json_tiny &j){
	return new VMWare(j);
}

void VMWare::CheckAllCustom() {
	bool d;
	std::pair<std::string, std::string> report;
	std::string ce_name;

	ce_name = Config::cvm2s[Config::ConfigVMWare::HYPERVISOR_PORT];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckHypervisorPort();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cvm2s[Config::ConfigVMWare::DEVICE_NPF_NDIS];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckNDISFile();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cvm2s[Config::ConfigVMWare::HYPERVISOR_BIT];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = IsHypervisor();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

}

bool VMWare::CheckHypervisorPort() const {
	bool is_vm = false;

	__try {
		__asm {
			push edx;
			push ecx;
			push ebx;
			mov eax, 'VMXh';
			mov ebx, 0;
			mov ecx, 10;
			mov edx, 'VX';
			in eax, dx;
			cmp ebx, 'VMXh';
			setz[is_vm];
			pop ebx;
			pop ecx;
			pop edx;
		}
	} 
	__except (EXCEPTION_EXECUTE_HANDLER) {
		is_vm = false;
	}

	return is_vm;
}

bool VMWare::CheckNDISFile() const {
	HANDLE hFile;
	const wchar_t ndis_wan_ip_fname[] = L"\\\\.\\NPF_NdisWanIp";
	DWORD err;

	hFile = CreateFileW(ndis_wan_ip_fname, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		err = GetLastError();
		return err != ERROR_PATH_NOT_FOUND && err != ERROR_FILE_NOT_FOUND;
	}

	return true;
}

bool VMWare::IsHypervisor() const {
	/*
	int32_t cpuinfo[4] = { 0 };

	__cpuid(cpuinfo, 0x00000000);

	return (cpuinfo[2] >> 31) & 1;
	*/
	return is_hypervisor();
}


} // SandboxEvasion