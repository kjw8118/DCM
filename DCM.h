#pragma once
#include "DCMExport.h"

#include <fstream>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include <GIT.h> // From github.com/kjw8118/GITwrap.git

#include "Element.h"

namespace DCM
{
	
	DCM_API std::vector<std::string> stripLine(std::string lineRaw);
	DCM_API int countDecimalPlaces(const std::string& numberStr);
	DCM_API std::string toFixed(double value, int precision);

	class DCM_API Manager
	{
	private:
		GIT* git = nullptr;
		std::string gitName = "";
		std::string gitEmail = "";

		bool isOpened = false;


		const std::string* rawString = nullptr;
		const std::vector<std::string>* rawStringList = nullptr;
		std::string getRawString();
		std::vector<std::string> getRawStringList();


		std::vector<std::string> lineHistory;
		int currentIndex = 0;
		int currentOrder = 0;
		Element* pCurrentElement = nullptr;

		std::vector<Element*> elements;
		std::vector<ModuleHeader*> moduleHeaders;
		std::vector<Comment*> comments;
		std::vector<Functions*> functions;
		std::vector<Unknown*> unknowns;
		std::vector<Format*> formats;
		std::vector<VariantCoding*> variantCodings;
		std::vector<BaseParameter*> parameters;

		std::string fPath = "";
		std::string fName = "";
		std::fstream file;
		
		std::map<std::string, Element*> elementIndex;
			
		void parseHeader(std::vector<std::string> lineStrip);
		void parseComponent(std::vector<std::string> lineStrip);
		void parseLine(std::string lineRaw);	


		void putElement(Element* element);

		int calcEndIndex(Element* element);

		
		std::string rebuildUnknown(Unknown* unknown);
		std::string rebuildFunctions(Functions* functions);
		std::string rebuildComment(Comment* comment);
		std::string rebuildVariantCoding(VariantCoding* variantCoding);
		std::string rebuildModuleHeader(ModuleHeader* moduleHeader);
		std::string rebuildFormat(Format* format);
		std::string rebuildParameter(Parameter* parameter);
		std::string rebuildBoolean(Boolean* boolean);
		std::string rebuildArray(Array* arr);
		std::string rebuildMatrix(Matrix* matrix);
		std::string rebuildLineBaseParameter(LineBaseParameter* line);
		std::string rebuildMapBaseParameter(MapBaseParameter* map);
		std::string rebuildDistribution(Distribution* dist);

		std::string rebuildElement(Element* element);
		
	public:
		Manager();
		void open(std::string _fPath);		
		void openWithRepo(std::string _fPath, std::string gitName, std::string gitEmail);
		void openWithRepo(std::string _fPath) { return openWithRepo(_fPath, gitName, gitEmail); };
		void cloneFromRepo(std::string _remotePath, std::string _localPath, std::string _gitName, std::string _gitEmail);

		void createDCM();
		void parse();
		bool isOpen() { return isOpened; };
		struct EditHistory
		{
			std::chrono::system_clock::time_point date;
			std::string message;
			std::string id;
			// Content from GIT::DiffHunk or std::vector<std::string> ?
			EditHistory() {};
			EditHistory(std::chrono::system_clock::time_point date, std::string message, std::string id)
				: date(date), message(message), id(id) {};
		};
		
		
		std::vector<BaseParameter*> collectAllTypeParameters();
		std::vector<Distribution*> collectAxispoint();
		std::vector<Array*> collectValueBlock();
		std::vector<Matrix*> collectValueMatrix();
		std::vector<Parameter*> collectValue();
		std::vector<Parameter*> collectValueBoolean();
		std::vector<LineBaseParameter*> collectCurve();
		std::vector<MapBaseParameter*> collectMap();

		

		void saveCurrentDCM()
		{
			if (file.is_open())
			{
				file.close();
			}
			std::ofstream outputFile(fPath, std::ios::trunc);
			if (!outputFile.is_open())
			{
				std::cout << "Save file can not open" << std::endl;
				return;
			}
			outputFile << rebuildElements();
			outputFile.close();
			std::cout << "Save finished " << fPath << std::endl;
		}

		void editContents(std::string edit_message)
		{
			if (git != nullptr)
			{
				git->gitAdd(fName);
				git->gitCommit(edit_message);
				std::cout << "Commit done " << edit_message << std::endl;
			}
		}

		void reviseContents(std::string revision_name_mixed)
		{
			if (git != nullptr)
			{
				auto revList = git->getAllBranchList();
				if (std::find(revList.begin(), revList.end(), revision_name_mixed) == revList.end())
				{
					git->createBranch(revision_name_mixed);					
				}
				git->switchBranch(revision_name_mixed);
			}
		}

		std::vector<GIT::DiffResult> getDiffWithCurrent();							/* get difference between file and current elements*/
		std::vector<GIT::DiffResult> getDiffWithEdit(std::string edit_id);			/* get difference between file and edit(commit) version elements */

		
		std::string rebuildElements();												/* rebuild full text from current elements */
		
		//std::string rebuildFromDiff(std::vector<GIT::DiffResult> diffResults);	/* rebuild full text from file contents with diff */
		//std::string rebuildFromEdit(std::string edit_id);							/* rebuild full text form file contents with commit diff */

		
		std::string getContentsFromFile();											/* get full text from file contents */	
		std::string getContentsFromEditID(std::string edit_id);						/* get full text from commit contents */
		std::string getContentsFromRevision(std::string revision);					/* get full text from branch contents */

		void loadContents(std::string contents);									/* clear, re-parsing and apply from full text */
		//void loadContentsFromFile();												/* clear, re-parsing and apply from file contents = loadContents(getContentsFromFile())*/
		//void loadContentsFromEdit(std::string edit_id);							/* clear, re-parsing and apply from commit contents = loadContents(getContentsFromEditID()) */

		void saveAsDCM(std::string fname);
		
		void clear();
		void close();
		std::string getFileName() { return fPath; };

		std::vector<Element*> getElements();
		Element* findElement(std::string name, bool exactmatch=false);
		std::vector<Element*> findElements(std::string name, bool exactmatch = false);
		std::vector<BaseParameter*> findBaseParameters(std::string name, bool exactmatch = false);
		
		Element* findElementFromIndex(int index);

		std::vector<EditHistory> getEditHistoryList();
		std::vector<std::string> getRevisionList();
		
		void removeElement(Element* element);
		void removeElement(int order);

		//std::string getContentsAtHistory(EditHistory editHistory);
		//std::string getContentsAtHistory(std::string editHistory_id);

		
		
		std::string generateComment(std::string title);
		
		bool isParameter(Element* element);
		

		//void forkBranch();
		std::vector<std::pair<bool, std::pair<BaseParameter*, BaseParameter*>>> compareWith(std::vector<BaseParameter*> &otherBaseParameters);
		void copyWith(std::vector<BaseParameter*>& otherBaseParameters);
		std::vector<std::pair<DCM::BaseParameter*, DCM::BaseParameter*>> pairBaseParametersWith(std::vector<BaseParameter*>& otherBaseParameters);
		static bool test();

		// Test Code
		static bool typeEnumTest();
		static bool stripQuotationTest();		
		static bool rebuildUnknownTest();
		static bool rebuildFunctionsTest();
		static bool rebuildCommentTest();
		static bool rebuildFormatTest();
		static bool rebuildParameterTest();
		static bool rebuildArrayTest();
		static bool rebuildMatrixTest();
		static bool rebuildLineBaseParameterTest();
		static bool rebuildMapBaseParameterTest();
		static bool rebuildDistributionTest();

		static bool parseDCM1Test();
		static bool parseDCM2Test();

		//static bool createDCMTest();
	};
};


