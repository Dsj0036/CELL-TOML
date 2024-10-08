#pragma once

/// 
/// toml.hpp 
/// written by jsh
/// documented by ai.
/// Provides basic utilities for CELL inspired by .NET
/// part of the PS3 Framework.
/// 


#include "System.h"
#include "text_reader.hpp"

namespace System {
	namespace Serialization {
		namespace TOMLANG {
			static const char* BOOLEAN_TRUE_LITERAL = "true";
			static const char* BOOLEAN_FALSE_LITERAL = "false";
			/// <summary>
			/// Enumeration of possible value kinds.
			/// </summary>
			enum Kind {
				Comment,
				Path,
				Bool,
				Integer,
				String,
				Array,
				Double,
				Unknown,
			};
			static const char* TOMLKindToString(Kind x) {
				switch (x)
				{
					RETNAMEOFINCASE(Comment);
					RETNAMEOFINCASE(Path);
					RETNAMEOFINCASE(Bool);
					RETNAMEOFINCASE(Integer);
					RETNAMEOFINCASE(String);
					RETNAMEOFINCASE(Array);
					RETNAMEOFINCASE(Unknown);

				}
				return "";
			}

			/// <summary>
			/// Enumeration of possible result cases.
			/// </summary>
			enum TOMLResultStatusCode {
				Overflow,
				Sucess, /// SUCESS
				SucessNowGotoNextLine, /// MUST BE ARGUMENTED ALONG WITH THE LENGTH OF THE RESULT
				UnexpectedToken,
				UnexpectedEOF,
				DuplicatedAssignmentOperator,
				NullReference,
				FloatingPointUnexpectedDecimalGap,
				InvalidFloatFormat,
				WrongPathFormat,
				PathNotFound,
				NotValidTryNext,
			};
			/// <summary>
			/// Get statically constant name for the specified status code.
			/// </summary>
			/// <param name="x">the code</param>
			/// <returns></returns>
			static const char* TOMLResultCodeToString(TOMLResultStatusCode x) {
				switch (x) {

					RETNAMEOFINCASE(Overflow);
					RETNAMEOFINCASE(Sucess);
					RETNAMEOFINCASE(SucessNowGotoNextLine);
					RETNAMEOFINCASE(UnexpectedToken);
					RETNAMEOFINCASE(UnexpectedEOF);
					RETNAMEOFINCASE(DuplicatedAssignmentOperator);
					RETNAMEOFINCASE(NullReference);
					RETNAMEOFINCASE(FloatingPointUnexpectedDecimalGap);
					RETNAMEOFINCASE(InvalidFloatFormat);
					RETNAMEOFINCASE(WrongPathFormat);
					RETNAMEOFINCASE(PathNotFound);
					RETNAMEOFINCASE(NotValidTryNext);
				}
				return "";
			}
			/// <summary>
			/// Operation result encapusulation quick
			/// </summary>
			struct TOMLResultStatus {
				TOMLResultStatusCode StatusCode;
				HResult Valuable;
				TOMLResultStatus(TOMLResultStatusCode x) : StatusCode(x), Valuable(0) {}
				TOMLResultStatus(TOMLResultStatusCode x, HResult xc) : StatusCode(x), Valuable(xc) {}
				TOMLResultStatus& operator =(TOMLResultStatus& x) {
					StatusCode = x.StatusCode;
					Valuable = x.Valuable;
					return *this;
				}
			};
			struct TOMLToken {
				char* contents;
				int length;
			};


			/// <summary>
			/// Represents a path pointer who haves a known length in a text pool.
			/// </summary>
			class PathName {
				/// <summary>
				/// [Not trivial] 
				/// </summary>
				char* pathName;
			public:
				/// <summary>
				/// Creates a root path instance
				/// </summary>
				PathName() {
					pathName = nullptr;
				}
				PathName(char* name, int length) {
					pathName = name;
				}
				/// <summary>
				/// Ensures the path is root (null).
				/// </summary>
				bool IsRoot() {
					return pathName == nullptr;
				}
				/// <summary>
				/// Copies the specified range of contents represented by this buffer
				/// </summary>
				/// <param name="buffer"></param>
				void Output(void* buffer) {
					int length = Text::IndexOf(pathName, ']');
					Marshal::Clear(buffer, length);
					Marshal::Copy(pathName, buffer, length);
				}
				/// <summary>
				/// [Factory] Build this instance as an root path descriptor.
				/// </summary>
				void Build() {
					pathName = nullptr;
				}
				/// <summary>
				/// [Factory] Build this instance as an specified path descriptor. 
				/// </summary>
				/// <param name="name">Path token</param>
				/// <param name="length">Path token length</param>
				void Build(char* name) {
					pathName = name;
				}
				/// <summary>
				/// Outputs the path name.
				/// </summary>
				/// <param name="buffer">Target buffer</param>
				/// <returns>True if sucess.</returns>
				Boolean getPathName(char* buffer) {
					if (pathName != nullptr && buffer != nullptr) {
						char* iterator = pathName;
						int length = 0;
						while (*iterator != ']' && *iterator != '\n' && *iterator != '\r') {
							length++;
							iterator++;
						}
						length--;
						Marshal::Copy(pathName, buffer, length);
						return true;
					}
					return false;
				}
				bool Equals(const char* name) const{
					return safest_compare(this->pathName, name);
				}
				/// <summary>
				/// The position of this path determined by this instance from the root contents.
				/// </summary>
				/// <returns>Nullable char array</returns>
				char* GetContents() const {
					return pathName;
				}
			};
			/// <summary>
			/// This is explicitly buildable because i dont trust in default constructors and assignment operators.
			/// </summary>
			class Value {
			public:

				// [Not trivial] 
				Kind kind;
				// [Not trivial] 
				TOMLToken token;
				void Output(const char*);
				/// [generator only]
				Value() {}
				Value& operator =(const Value& ref) {
					this->kind = ref.kind;
					this->token = ref.token;
					return *this;
				}
				/// <summary>
				/// [Factory] Builds this instance as an null value descriptor.
				/// </summary>
				void Build() {
					kind = Unknown;
					token.contents = 0;
					token.length = 0;
				}
				/// <summary>
				/// Force explicitly data rebuild.
				/// </summary>
				/// <param name="kind"></param>
				/// <param name="token"></param>
				void Build(Kind kind, char* start, int length) {
					this->kind = kind;
					this->token.contents = start;
					this->token.length = length;
				}
				/// <summary>
				/// Clears all the non trivial values of this instance.
				/// </summary>
				~Value() {
					Marshal::Clear(this, sizeof(*this));
				}
			};
			/// <summary>
			/// Default entry key-value pair holder model. 
			/// </summary>
			class Entry {
			public:
				/// <summary>
				/// [Not trivial] Storage of this element's path on the root document.
				/// </summary>
				PathName* path;
				/// <summary>
				/// [Not Trivial] Entry value descriptor
				/// </summary>
				Value value;
				/// <summary>
				/// [Factory] Build specifically this instance
				/// </summary>
				/// <param name="path"></param>
				/// <param name="kind"></param>
				/// <param name="token"></param>
				Entry(PathName* path, Kind kind, TOMLToken token) {
					this->path = path;
					value.kind = kind;
					value.token = token;
				}
				/// <summary>
				/// [Factory] Build this instance as an incompleted or in-processing entry.
				/// </summary>
				Entry() {
					value.Build();
				}
				/// <summary>
				/// Outputs the valuable text of this string.
				/// </summary>
				/// <param name="buffer"></param>
				bool Output(char* buffer) {
					int index = Text::IndexOf(value.token.contents, '=');
					if (index != -1) {
						char * data = value.token.contents + index + 1;
						while (*data == ' ') {
							data++;
						}
						int ln = Text::LineLength(data);
						Marshal::Clear(buffer, ln);
						Marshal::Copy(data, buffer, ln);
						return true;
					}
					return false;
				}
				/// <summary>
				/// Checks if this instance belongs to the document's root path.
				/// </summary>
				/// <returns>Boolean</returns>
				bool isRootPath() {
					if (path) {
						return path->IsRoot();
					}
					return path == nullptr;
				}
				int32_t getInt() {
					char buffer[32];
					if (!Output(buffer)) {
						return 0;
					}
					signed int value = Int::Parse(buffer);
					Marshal::Clear(buffer);
					return value;
				}
				double getDecimal() {
					char buffer[32];
					if (!Output(buffer)) {
						return 0.0;
					}
					double value = Double::Parse(buffer);
					Marshal::Clear(buffer);
					return value;
				}
				float getFloat() {
					char buffer[32];
					if (!Output(buffer)) {
						return 0.0f;
					}
					float value = Float::Parse(buffer);
					Marshal::Clear(buffer);
					return value;
				}
				signed char getBoolean() {
					char buffer[32];

					if (!Output(buffer)) {
						return 0.0f;
					}
					signed char value = Boolean::Parse(buffer);
					Marshal::Clear(buffer);
					return value;
				}
				/// <summary>
				/// Clears all the non-trivial values determined by this class.
				/// </summary>
				~Entry() {
					Marshal::Clear(this, sizeof(*this));
				}
			};
			/// <summary>
			/// Comment entry holder class. Alias for an token.
			/// </summary>
			class CommentEntry {
			public:
				int index;
				int length;
				CommentEntry() {}
				CommentEntry(int c, int l) : index(c), length(l) {}
				~CommentEntry() {
					Marshal::Clear(this, sizeof(*this));
				}
			};
			/// <summary>
			/// Represents the main root contents and container of a TOML document.
			/// </summary>
			class Root {
				int idxPaths = 0;
				int idxEntries = 0;
				int idxComments = 0;
			public:
				/// <summary>
				/// Factory Initialize
				/// </summary>
				Root(no_init) {
				}
				Root() {
					idxPaths = 0;
					idxEntries = 0;
					idxComments = 0;
				}
				
				PathName* Paths;
				Entry* Entries;
				CommentEntry* Commentaries;
				/// <summary>
				/// The document original data.
				/// </summary>
				char* Data;
				/// <summary>
				/// Get the count of the collected entries.
				/// </summary>
				/// <returns>integer</returns>
				int getLength() const{
					return idxEntries;
				}
				/// <summary>
				/// Get the count of the collected paths.
				/// </summary>
				/// <returns>integer</returns>
				int pathCount()const {
					return idxPaths;
				}
				/// <summary>
				/// Get the count of the collected comments.
				/// </summary>
				/// <returns>integer</returns>
				int commentCount() const {
					return idxComments;
				}
				/// <summary>
				/// Push an entry.
				/// </summary>
				/// <param name="entryModelInstance"></param>
				void AddEntry(Entry entryModelInstance) {
					//Contents.Push(entryModelInstance);
					Entries[idxEntries].path = entryModelInstance.path;
					Entries[idxEntries].value = entryModelInstance.value;
					idxEntries++;
				}
				/// <summary>
				/// [Generation only] Register an specified entry
				/// </summary>
				/// <param name="entryModelInstance"></param>
				void AddEntry(PathName* path, Kind kind, TOMLToken token) {
					Entry entry(path, kind, token);
					AddEntry(entry);
				}
				/// <summary>
				///  [Generation only] Register an specified entry (2)
				/// </summary>
				/// <param name="path"></param>
				/// <param name="kind"></param>
				/// <param name="token"></param>
				void AddEntryAndPath(PathName& path, Kind kind, TOMLToken token) {
					for (int i = 0; i < idxPaths; i++) {
						if (Paths[i].GetContents() == path.GetContents()) {
							AddEntry(&(Paths[i]), kind, token);
							return;
						}
					}
					Paths[idxPaths] = path;
					AddEntry(&Paths[idxPaths], kind, token);
					idxPaths++;
				}
				/// <summary>
				/// [Generation Only] Register an path if isnt already
				/// </summary>
				/// <param name="path"></param>
				void AddPath(PathName& path) {
					PathName* iterator = this->Paths;

					for (int i = 0; i < idxPaths; i++) {
						if (!iterator[i].IsRoot()) {

							if (strncmp(iterator[i].GetContents(), path.GetContents(), Text::IndexOf(iterator[i].GetContents(), ']')) == 0) {
								return;
							}
						}
					}
					Paths[idxPaths++] = path;
				}
				/// <summary>
				/// [Generation only] Register an comment
				/// </summary>
				/// <param name="tokenStart"></param>
				/// <param name="tokenLength"></param>
				void AddComment(int tokenStart, int tokenLength) {
					Commentaries[idxComments].index = tokenStart;
					Commentaries[idxComments].length= tokenLength;
					idxComments++;
				}
				/// <summary>
				/// [Generation only] Set the originary data.
				/// </summary>
				/// <param name="data">Char array</param>
				void SetData(char* data) {
					this->Data = data;
				}
				
				/// <summary>
				/// Destroy this instance
				/// </summary>
				void Destroy() {
					delete[] Paths;
					delete[] Entries;
					delete[] Commentaries;
					sys::memset(Commentaries, 0, sizeof(CommentEntry) * idxComments);
					sys::memset(Paths, 0, sizeof(PathName) * idxPaths);
					sys::memset(Entries, 0, sizeof(Entry) * idxEntries);
				}
				void Initialize(int paths, int entries, int comments) {
					Paths = new PathName[paths];
					Entries = new Entry[paths];
					Commentaries = new CommentEntry[paths];
				}
				/// <summary>
				/// Let him destroy this instance manually.
				/// </summary>
				~Root() {
					__nop();
				}
			};
			typedef HResult TOMLHResultOrPtr;

			/// <summary>
/// Encapsulates a dynamic instance of a TOML enumeration
/// </summary>
			class TOML {
			public:
				/// <summary>
				/// Quick factory initialization
				/// </summary>
				/// <param name="data">Pointer to the raw TOML data</param>
				TOML(char* data) {
					Contents.ConstructorInit();  // Initialize the instance of Root container
					Contents->SetData(data);     // Set the provided TOML data to the instance
				}

				/// <summary>
				/// Default constructor, no initialization.
				/// </summary>
				TOML() {

				}

				/// <summary>
				/// Dynamic and shared instance of the root container.
				/// </summary>
				System::Instance<Root> Contents;

				/// <summary>
				/// Get the count of the collected entries within the TOML structure.
				/// </summary>
				/// <returns>Number of entries in the TOML</returns>
				size_t Count() {
					if (Contents.NotNull()) {
						return Contents->getLength();  // Returns the length of the entries in the root container
					}
					return 0;
				}
				/// <summary>
				/// Get the used memory size by the root container of the document contents.
				/// </summary>
				/// <returns>size_t</returns>
				size_t SizeOf() {
					return
						Contents->getLength() * sizeof(Entry) |
						Contents->commentCount() * sizeof(CommentEntry) |
						Contents->pathCount() * sizeof(PathName) | 
						sizeof(Root);
				}
				/// <summary>
				/// Index accessor to retrieve the entry at a specific index.
				/// </summary>
				/// <param name="index">Index of the entry</param>
				/// <returns>A reference to the Entry object at the given index</returns>
				Entry& operator [](size_t index) {
					return Contents->Entries[index];  // Access the entry at the specified index
				}

				/// <summary>
				/// Get the original text data that was used to initialize the TOML object.
				/// </summary>
				/// <returns>Pointer to the raw data (nullable)</returns>
				char* GetText() {
					return Contents->Data;  // Return the original data used for parsing
				}

				/// <summary>
				/// Find a path by its name within the TOML structure.
				/// </summary>
				/// <param name="name">Name of the path to search for</param>
				/// <returns>Pointer to the matching PathName object or nullptr if not found</returns>
				PathName* FindPathByName(const char* name) {
					forIndexIn(Contents->pathCount()) {  // Loop through all paths
						if (Contents->Paths[i].Equals(name)) {  // Compare names
							return &Contents->Paths[i];  // Return the matching path
						}
					}
					return nullptr;  // Return null if no path matches
				}

				/// <summary>
				/// Find an entry by its full path within the TOML structure.
				/// </summary>
				/// <param name="fullpath">Full path string to the entry (e.g., "path/to/entry")</param>
				/// <returns>Pointer to the matching Entry object or nullptr if not found</returns>
				Entry* FindEntryByPath(const char* fullpath) {
					if (!fullpath) {  // Ensure fullpath is valid
						return nullptr;
					}
					if (sys::strlen(fullpath) > 0) {
						Entry* result = nullptr;
						if (Text::IndexOf(fullpath, '/') != -1) {  // Check if the path contains a slash (indicating a hierarchical path)
							char pathBuff[64];
							char entryName[64];
							if (Text::SplitLineByKey(fullpath, '/', pathBuff, entryName)) {  // Split the path and entry name
								for (int i = 0; i < Contents->getLength(); i++) {  // Loop through the entries
									if (safest_compare(Contents->Entries[i].value.token.contents, entryName) &&
										Contents->Entries[i].path->Equals(pathBuff)) {
										result = &Contents->Entries[i];  // Set result if entry and path match
									}
								}
							}
							Marshal::Clear(pathBuff);  // Clear the buffer for security
							Marshal::Clear(entryName);
							return result;  // Return the found entry
						}
						else {
							for (int i = 0; i < Contents->getLength(); i++) {  // If no slash, look for a direct entry match
								if (safest_compare(Contents->Entries[i].value.token.contents, fullpath)) {
									result = &Contents->Entries[i];
								}
							}
							return result;  // Return the found entry
						}
					}
					return nullptr;  // Return null if no entry matches
				}

				/// <summary>
				/// Destroy this instance of TOML, clearing its contents.
				/// </summary>
				void Destroy() {
					Contents->Destroy();  // Clear the root container
					Contents.Destroy();   // Destroy the instance itself
				}

				/// <summary>
				/// Destructor for TOML instance.
				/// </summary>
				~TOML() {
					__nop();  // No operation, placeholder for any future cleanup
				};
			};

			class Parser {
			public:

				// [Obsolete]
				template <typename prim>
				static bool IsFlaggedBy(prim& integerType, uint16_t bits) {
					return (integerType & bits) != 0;
				}
				// [Obsolete]
				template <typename prim>
				static void SetFlag(prim& integerType, uint16_t bits, bool switchable) {
					if (switchable)
						integerType |= bits;
					else
						integerType &= ~bits;
				}

				static Boolean IsKeyValidCharacter(Char c) {
					return
						c.IsLetter() ||
						c.IsDigit() ||
						c.Equals('-') ||
						c.Equals('_');
				}
				/// <summary>
				/// Parses the valuable decimal (float/double) side of the equation.
				/// </summary>
				/// <param name="startIndex">Reference index of the line, for the final token to be stored in output</param>
				/// <param name="segmentIterator">Data accessor iterator starting after the equal symbol.</param>
				/// <param name="output">Output structure space for storing resulting analysis if operation completes.</param>
				/// <returns></returns>
				static TOMLResultStatus ParseDecimal(char* segmentIterator, Value& output) {
					char* backup = segmentIterator;
					int numerics = 0;
					char* floatValuableBegin = nullptr;
					bool foundDecimalGap = false;
					while (*segmentIterator && *segmentIterator != '#' && *segmentIterator != '\n') {
						Char current = *segmentIterator;
						if ((current.Equals('.') && (!foundDecimalGap)) && numerics == 0) {
							/// ERROR: TEXT CANNOT START WITH DECIMAL GAP.
							return InvalidFloatFormat;
						}
						else if ((current.Equals('.') && (!foundDecimalGap)) && numerics > 0) {
							foundDecimalGap = true;
						}
						else if ((current.Equals('.') && (foundDecimalGap)) && numerics > 0) {
							return { FloatingPointUnexpectedDecimalGap };
						}
						else if (current.IsDigit()) {
							if (floatValuableBegin == nullptr) {
								floatValuableBegin = segmentIterator;
							}
							numerics++;
						}
						else if (current.IsLetter()) {
							/// ERROR: TEXT CANNOT CONTAINS LETTERS.
							return InvalidFloatFormat;
						}
						segmentIterator++;
					}
					if (floatValuableBegin && numerics > 0) {

						char buffer[32];
						for (int i = 0; i < numerics; i++) {
							buffer[i] = floatValuableBegin[i];
						}
						return Text::couldBeFloatValueRepresentation(buffer, '.', false) ? Sucess : InvalidFloatFormat;
					}
					return InvalidFloatFormat;
				}
				/// <summary>
				/// Parses the valuable numeric (int) side of the equation.
				/// </summary>
				/// <param name="startIndex">Reference index of the line, for the final token to be stored in output</param>
				/// <param name="segmentIterator">Data accessor iterator starting after the equal symbol.</param>
				/// <param name="output">Output structure space for storing resulting analysis if operation completes.</param>
				/// <returns></returns>
				static TOMLResultStatus ParseNumeric(char* segmentIterator, Value& output) {
					char* iterator = segmentIterator;
					while (*segmentIterator == ' ') {
						segmentIterator++;
					}
					char* valuableBegin = nullptr;
					int numerics = 0;
					while (*segmentIterator != '#' && *segmentIterator != '\n' && *segmentIterator != '\r') {
						Char cc = *segmentIterator;
						if (cc.IsDigit()) {
							if (valuableBegin == nullptr) {
								valuableBegin = segmentIterator;
							}
							numerics++;
						}
						else if (cc.Equals(' ') && numerics > 0) { // avoid starting with spaces or after weird symbols
							break;
						}
						else {
							return { UnexpectedToken, 0 };
						}
						segmentIterator++;
					}
					if (valuableBegin != nullptr) {
						char buffer[32];
						sys::memset(buffer, 0, 32);
						for (int i = 0; i < numerics; i++) {
							buffer[i] = valuableBegin[i];
						}
						auto res = Text::couldBeIntegerRepresentation(buffer) ? Sucess : NotValidTryNext;;
						if (res == Sucess) {
							output.Build(Kind::Integer, valuableBegin, numerics);
						}
						return res;
					}
					return NotValidTryNext;
				}
				/// <summary>
				/// Parses the valuable boolean (bool) side of the equation.
				/// </summary>
				/// <param name="startIndex">Reference index of the line, for the final token to be stored in output</param>
				/// <param name="segmentIterator">Data accessor iterator starting after the equal symbol.</param>
				/// <param name="output">Output structure space for storing resulting analysis if operation completes.</param>
				/// <returns></returns>
				static TOMLResultStatus ParseBoolean(char* segmentIterator, Value& output) {
					char* booleanIterator = segmentIterator;
					char* resultLocation = nullptr;
					int length = 0;

					while (*booleanIterator != '#' && *booleanIterator && *booleanIterator != '\n' && *booleanIterator != '\r') {

						bool containsPhrase = Text::StartsWith(booleanIterator, "true") || Text::StartsWith(booleanIterator, "false");
						if (containsPhrase) {
							resultLocation = booleanIterator;
							break;
						}
						length++;
						booleanIterator++;
					}
					if (resultLocation) {
						char* phraseEndingCheckIterator = resultLocation;
						byte TrailingSpacesButItsOk = 0;
						bool correctEnding = false;
						while (Char(*phraseEndingCheckIterator).IsLetter() &&
							*phraseEndingCheckIterator != '#' &&
							*phraseEndingCheckIterator != '-') {

							if (*phraseEndingCheckIterator == '\n') {
								correctEnding = true;
								break;
							}
							else if (*phraseEndingCheckIterator == ' ') {
								TrailingSpacesButItsOk++;
							}
							else {
								output.Build(Kind::Bool, resultLocation, 6);
								return { Sucess };
							}
							phraseEndingCheckIterator++;
						}

						return { Sucess, TrailingSpacesButItsOk };
					}
					return NotValidTryNext;
				}
				/// <summary>
				/// Parses the valuable text (string) side of the equation.
				/// </summary>
				/// <param name="startIndex">Reference index of the line, for the final token to be stored in output</param>
				/// <param name="segmentIterator">Data accessor iterator starting after the equal symbol.</param>
				/// <param name="output">Output structure space for storing resulting analysis if operation completes.</param>
				/// <returns></returns>
				static TOMLResultStatus ParseString(char* segmentIterator) {

					char* stringInitialIterator = segmentIterator;
					char* stringIterator = segmentIterator;
					char* stringStart = nullptr;
					char* stringEnd = nullptr;

					while (*stringIterator != '\n' && *stringIterator != '#' && *stringIterator != '\r') {
						if (*stringIterator == '"' && stringStart == nullptr) {
							stringStart = stringIterator + 1;
						}
						else if (*stringIterator == '"' && stringStart && stringEnd == nullptr) {
							stringEnd = stringIterator;
						}
						else if (*stringIterator == '"' && stringStart && stringEnd) {
							return { UnexpectedToken, 0 };
						}

						stringIterator++;
					}
					if (stringStart && stringEnd) {
						return { Sucess, stringEnd - stringStart };
					}
					return { TOMLResultStatusCode::NotValidTryNext, 0 };
				}
				/// <summary>
				/// Parses the valuable equation side of the equation.
				/// </summary>
				static HResult ParseEquation(char* contents, Value& output, TOMLResultStatus* outputResult) {
					// Paso 1: encontrar el símbolo de asignación '='
					char* iterator = contents;
					bool foundAssignment = false;
					while (*iterator != '\n' && *iterator != '\0') { // Displace until end of line or eof for finding assignment operator.
						if (*iterator == '=') { // Operator has been found break and start analysis.
							foundAssignment = true;
							break;
						}
						iterator++;
					}
					if (foundAssignment) {
						int length = 0;
						iterator++; // Move to the next char ignoring the starting '='
						while (*iterator == ' ') { // Displace for all initially trailling space.
							iterator++;
						}
						int EOL_Offset = 0;
						bool commentFound = false;
						char* defaultValuableIterator = iterator;
						char* commentIterator = iterator;
						char* integerIterator = iterator;
						char* booleanIterator = iterator;
						char* decimalIterator = iterator;
						/// TRY FIND COMMENTARY
						while (*commentIterator && *commentIterator != '\n') {
							if (*commentIterator == '#') {
								commentFound = true;
							}
							commentIterator++;
							EOL_Offset++;
						}
						/// TRY TO PARSE THE VALUABLE SEGMENT INTO AN STRING
						TOMLResultStatus stringAnalysis = ParseString(defaultValuableIterator);
						*outputResult = stringAnalysis;
						if (stringAnalysis.StatusCode == Sucess) {
							/// IN THIS CASE, WE ASSIGNED ALREADY THE LENGTH OF THE RESULT INTO THE RESULT CODE. 
							output.Build(Kind::String, defaultValuableIterator, stringAnalysis.Valuable);
							return Sucess;
						}
						else {
							TOMLResultStatus booleanAnalysis = ParseBoolean(booleanIterator, output);
							*outputResult = booleanAnalysis;
							if (booleanAnalysis.StatusCode == Sucess) {
								return Sucess;
							}
							else {
								TOMLResultStatus integralAnalysis = ParseNumeric(integerIterator, output);
								*outputResult = integralAnalysis;
								if (integralAnalysis.StatusCode == Sucess) {

									return Sucess;
								}
								else {
									TOMLResultStatus decimalAnalysis = ParseDecimal(decimalIterator, output);
									*outputResult = decimalAnalysis;
									if (decimalAnalysis.StatusCode == Sucess) {

										return Sucess;
									}
									else {
										return EUNEXPECTED;
									}
								}
							}
						}
					}
					else {
						return EINVALID;
					}

				}
				/// <summary>
				/// Tries to determine the kind of the TOML line, then stores the information into 'output'
				/// using the provided index, length and line information.
				/// </summary>
				/// <param name="ReferenceIndex">Character index of the current line</param>
				/// <param name="line">The line section after the break</param>
				/// <param name="lineLength">The length of the line until nullterminator or break</param>
				/// <param name="output">Target resulting output</param>
				/// <param name="outputStatus">Target resulting status</param>
				/// <returns></returns>
				static Kind ParseLine(char* line, size_t lineLength, Value& output, TOMLResultStatus* outputStatus) {
					if (!line) {
						return Kind::Unknown;
					}
					while (*line == '#') {
						output.kind = Kind::Comment;
						output.token.contents = line;
						output.token.length = Text::LineLength(line);

						return Kind::Comment;
						break;
					}
					char* original = line;
					char* iterator = line;
					while (*iterator == ' ') {
						iterator++;
					}
					if (*iterator == '[' && *(iterator + lineLength - 1) == ']') {
						output.kind = Path;
						output.token.contents = iterator + 1; 
						output.token.length = lineLength - 1;
						return output.kind;
					}
					else {
						HResult h = ParseEquation(line, output, outputStatus);
						if (h == 1 || h == 0) {
							return output.kind;
						}
						return Unknown;
					}

				}

				static Kind GetKindForPairValueString(char* valueString) {
					int length = Text::LineLength(valueString);
					int index = 0;
					while (*valueString == ' ') {
						index++;
						valueString++;
					}
					if (safest_compare(valueString, BOOLEAN_FALSE_LITERAL) ||
						safest_compare(valueString, BOOLEAN_TRUE_LITERAL)) {
						return Kind::Bool;
					}
					else {
						int letters_c = Text::countLettersInLine(valueString);
						int numbers_c = Text::countNumbersInLine(valueString);
						int spaces_c = Text::countSpacesInLine(valueString);
						if (letters_c == 0 && numbers_c > 0) {
							if (Text::couldBeIntegerRepresentation(valueString)) {
								return Kind::Integer;
							}
							else if (Text::couldBeFloatValueRepresentation(valueString, '.')) {
								return Kind::Double;
							}
						}
						if (*valueString == '[') {
							return Kind::Array;
						}
						else {
							char commaEnding = 0;
							char commaStarting = 0;
							int textEnd = 0;
							int textStart = 0;
							for (int i = length - 1; i > 0; i--) {
								if (valueString[i] == '"') {
									commaEnding = '"';
									textEnd = i;
									break;
								}
								else if (valueString[i] == '\'') {
									commaEnding = '\'';
									textEnd = i;
									break;
								}
							}
							for (int i = 0; i < length; i++) {
								if (valueString[i] == '"') {
									commaStarting = '"';
									textStart = i;
									break;
								}
								else if (valueString[i] == '\'') {
									commaStarting = '\'';
									textStart = i;
									break;
								}
							}
							if (commaEnding == commaStarting && (commaStarting > 0 && commaEnding > 0)) {
								return Kind::String;
							}
							return Kind::Unknown;
						}
					}

				}

				static Boolean Parse(char* content, size_t content_length, TOML* toml) {
					if (!content) {
						return false;
					}
					Reader textReader{};
					textReader.SetContent(content);
					int iteratedLines = 0;
					size_t length = 0;
					PathName currentPath;
					TOMLResultStatus statusLine(TOMLResultStatusCode::NullReference, 0);
					
					int paths_c = 0;
					int comments_c = 0;
					int entries_c = 0;

					while (!textReader.IsEof()) {
						char* current = textReader.Current();
						size_t index = current - content;
						while ((*(current + 1) == '\n' && (*current) == '\n') || (*(current + 1) == '\r' && (*current) == '\r')) {
							current = textReader.NextLine(length);
						}
						while (*current == ' ') {
							current++;
						}

						if (*current == '[') {
							paths_c++;
						}
						else if (*current == '#') {
							comments_c++;
						}
						else {
							entries_c++;
						}
						textReader.NextLine(length);
					}
					toml->Contents->Initialize(paths_c, entries_c, comments_c);

					textReader.Restart();


					while (!textReader.IsEof()) {
						char* current = textReader.Current();
						size_t index = current - content;
						while ((*(current + 1) == '\n' && (*current) == '\n') || (*(current + 1) == '\r' && (*current) == '\r')) {
							currentPath.Build(); // clear current path.
							current = textReader.NextLine(length);
							iteratedLines++;
						}
						while (*current == ' ') {
							current++;
						}
						if (*current == '#') {
							char* commentIterator = current + 1;
							int length2 = 0;
							while (Char(*commentIterator).IsLetter() && *commentIterator != '\n' && *commentIterator != '\n') {
								length2++;
								commentIterator++;
							}
							toml->Contents->AddComment(current - content, length2);
						}
						else if (*current == '[') {
							currentPath.Build(current + 1);
							toml->Contents->AddPath(currentPath);
						}
						else {
							Value valuable;
							
							signed int keyIndex = Text::IndexOf(current, '=');
							if (keyIndex != -1 && (keyIndex < 0xff)) {
								char key_str[64];
								char value_str[64];
								if (Text::SplitLineByKey(current, '=', key_str, value_str)) {
									Kind valueKind = GetKindForPairValueString(value_str);
									toml->Contents->AddEntryAndPath(currentPath, valueKind, { current, Text::LineLength(current)});
									__nop();
								}
								Marshal::Clear(key_str);
								Marshal::Clear(value_str);
							}

						}
						textReader.NextLine(length);
					}
					return true;
				}

			};
		}
	}
	
}
