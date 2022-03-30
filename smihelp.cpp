// smihelp.cpp : Defines the entry point for the application.
//

#include "smihelp.h"
#include <unordered_map>
#include <fstream>
#include <sstream>

using namespace std;

typedef unordered_map<string, string> Args;

bool DEBUG = false;

const string MODE_RM_EMPTY_LINES = "--rmempty",
MODE_FORMAT_TAB = "--tab2smi",
MODE_EMBBED_SMI = "--sdfsmi",
MODE_HELP = "--help";

const string PARAM_FILE_INPUT = "--in",
PARAM_FILE_OUTPUT = "--out",
PARAM_FORMAT = "--format",
PARAM_DELIMITER = "--delim",
PARAM_DEBUG = "--debug",
PARAM_SDF = "--sdf",
PARAM_SMI = "--smi";

const string VAL_TAB = "\t",
VAL_ENDL = "\n";

vector<string> ALL_MODES = { MODE_RM_EMPTY_LINES, MODE_FORMAT_TAB, MODE_EMBBED_SMI, MODE_HELP };
vector<string> ALL_PARAMS = { PARAM_FILE_INPUT, PARAM_FILE_OUTPUT, PARAM_FORMAT, PARAM_DELIMITER, PARAM_DEBUG, PARAM_SDF, PARAM_SMI };

// '@' or '[' ']' -> "do not include isotopic or chiral marking in OpenBabel"
vector<char> FORBIDDEN_CHARACTER_DP = { '@', '[', ']' };
bool containsDeepPurposeIllegalCharacters = false;

void log(string msg) {
	if (DEBUG) {
		cout << "[DEBUG> " << msg << endl;
	}
}

void error(string err_msg, int err_code) {
	cout << "Error : " << err_msg << endl;
	exit(err_code);
}

bool is_string_empty(string str) {
	for (char c : str) {
		if (c != '\t' && c != ' ') {
			return false;
		}
	}
	return true;
}

vector<string> split(string str, string del) {
	string strcopy = str;
	vector<string> res;
	size_t pos;
	while ((pos = strcopy.find(del)) != string::npos) {
		res.push_back(strcopy.substr(0, pos));
		strcopy.erase(0, pos + del.length());
	}
	res.push_back(strcopy.substr(0, pos));
	return res;
}

Args map_args(int argc, char** argv) {
	unordered_map<string, string> args;
	for (int i = 1; i < argc; i++) {
		const string arg = argv[i];
		if (*find(ALL_MODES.begin(), ALL_MODES.end(), arg) == arg) {
			continue;
}
		if (*find(ALL_PARAMS.begin(), ALL_PARAMS.end(), arg) != arg) {
			error("Unknown parameter : " + arg, 1);
		}
		else {
			if (arg == PARAM_DEBUG) {
				continue;
			}
			else if (args.find(arg) == args.end()) {
				log("Registered argument : " + arg + " = " + argv[i + 1]);
				args[arg] = argv[++i];
			}
			else {
				error("Duplicate argument : " + arg, 1);
			}
		}
	}
	log("Mapped arguments");
	return args;
}

void check_DeepPurpose_validity(string str) {
	for (char c : str) {
		if (*find(FORBIDDEN_CHARACTER_DP.begin(), FORBIDDEN_CHARACTER_DP.end(), c) == c) containsDeepPurposeIllegalCharacters = true;
	}
}

void show_help() {
	cout << "smihelp is a small helper program designed to help manipulating SMILES molecules \n\
	\n\
	\t--help : show help \n\n\
	\t--rmempty : remove empty lines \n\
		\t\t--in <file in> \n\
		\t\t--out <file out> \n\n\
	\t--tab2smi : convert tab file to smi \n\
		\t\t--in <file in> \n\
		\t\t--out <file out> \n\
		\t\t--format <format> : columns to keep (e.g. \"3,1\" : columns 3 then 1) \n\n\
	\t--sdfsmi : embbed smiles into sdf file \n\
		\t\t--sdf <sdf file in> \n\
		\t\t--smi <smi file in> \n\
		\t\t--out <file out> \n";
}

void rm_empty_lines(Args args) {
	log("Entered mode rmempty");
	ifstream in(args[PARAM_FILE_INPUT]);
	ofstream out(args[PARAM_FILE_OUTPUT]);
	string line;
	int empty_lines = 0;
	while (getline(in, line)) {
		if (is_string_empty(line)) {
			empty_lines++;
		}
		else {
			out << line << endl;
			check_DeepPurpose_validity(line);
		}
	}
	if (empty_lines > 0) {
		cout << "Removed " << empty_lines << " empty lines." << endl;
	}
	else {
		cout << "No empty lines !";
	}
	out.close();
	in.close();
}

void format_tab(Args args) {
	log("Entered mode tab2smi");
	ifstream in(args[PARAM_FILE_INPUT]);
	ofstream out(args[PARAM_FILE_OUTPUT]);
	string delimiter = args[PARAM_DELIMITER];
	if (delimiter == "\\t") delimiter = "\t";
	else if (delimiter == "\\n") delimiter = "\n";
	vector<string> format = split(args[PARAM_FORMAT], ",");
	vector<int> columns;
	for (int i = 0; i < format.size(); i++) {
		int col = stoi(format[i]);
		columns.push_back(col);
	}
	int line_count = 0;
	string line;
	while (getline(in, line)) {
		line_count++;
		check_DeepPurpose_validity(line);
		vector<string> res = split(line, "\t");
		for (int i = 0; i < columns.size(); i++) {
			if (i > 0) out << delimiter;
			out << res[columns[i]-1];
		}
		out << endl;
	}
	cout << "Formatted " << line_count << " lines as " << args[PARAM_FORMAT] << "." << endl;
	out.close();
	in.close();
}

void embbed_smi(Args args) {
	ifstream in_sdf(args[PARAM_SDF]);
	ifstream in_smi(args[PARAM_SMI]);
	ofstream out(args[PARAM_FILE_OUTPUT]);
	string line;
	int smi_num = 0;
	int missing_smiles = 0;
	vector<string> smiles;
	while (getline(in_smi, line)) smiles.push_back(split(line, "\t")[0]);
	while (getline(in_sdf, line)) {
		if (line.find("$$$$") != string::npos) {
			check_DeepPurpose_validity(smiles[smi_num]);
			missing_smiles += is_string_empty(smiles[smi_num]);
			out << ">  <Smiles>" << endl;
			out << smiles[smi_num++] << endl << endl << "$$$$" << endl;
		}
		else {
			out << line << endl;
		}
	}
	cout << "Embedded " << smiles.size() << " smiles into matching sdf file." << endl;
	cout << "There were " << missing_smiles << " missing smiles (empty lines)." << endl;
	out.close();
	in_smi.close();
	in_sdf.close();
}

int main(int argc, char** argv)
{
	if (argc == 1) {
		show_help();
	}
	else {
		string mode = argv[1];
		for (int i = 1; i < argc; i++) {
			string arg = argv[i];
			if (arg == PARAM_DEBUG) {
				DEBUG = true;
			}
		}
		if (*find(ALL_MODES.begin(), ALL_MODES.end(), mode) != mode) {
			error("Unrecognized argument : " + mode, 1);
		}
		log("Mode : " + mode);
		unordered_map<string, string> args = map_args(argc, argv);
		if (mode == MODE_HELP) {
			show_help();
		}
		else if (mode == MODE_RM_EMPTY_LINES) {
			rm_empty_lines(args);
		}
		else if (mode == MODE_FORMAT_TAB) {
			format_tab(args);
		}
		else if (mode == MODE_EMBBED_SMI) {
			embbed_smi(args);
		}
		if (containsDeepPurposeIllegalCharacters) {
			cout << "Warning : file contains character that are not supported by DeepPurpose !";
		}
	}
}
