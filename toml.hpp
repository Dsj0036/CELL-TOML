#pragma once
#include "System.h"
#include "text_reader.hpp"

namespace System {
	namespace Serialization {
		namespace TOML {
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
				Unknown,
			};
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
			/// <summary>
			/// Represents a path pointer who haves a known length in a text pool.
			/// </summary>
			class PathName {
				/// <summary>
				/// [Not trivial] 
				/// </summary>
				char* pathName;
				/// <summary>
				/// [Not trivial] 
				/// </summary>
				int pathLength;
			public:
				/// <summary>
				/// Creates a root path instance
				/// </summary>
				PathName() {
					pathName = nullptr;
					pathLength = 0;
				}
				PathName(char* name, int length) {
					pathName = name;
					pathLength = length;
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
					Marshal::Clear(buffer, pathLength);
					Marshal::Copy(pathName, buffer, pathLength);
				}
				/// <summary>
				/// [Factory] Build this instance as an root path descriptor.
				/// </summary>
				void Build() {
					pathName = nullptr;
					pathLength = 0;
				}
				/// <summary>
				/// [Factory] Build this instance as an specified path descriptor. 
				/// </summary>
				/// <param name="name">Path token</param>
				/// <param name="length">Path token length</param>
				void Build(char* name, int length) {
					pathName = name;
					pathLength = length;
				}
				/// <summary>
				/// Length of the path token
				/// </summary>
				/// <returns>Int</returns>
				int Size() const {
					return pathLength;
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
				StringToken token;
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
					token = StringToken();
				}
				/// <summary>
				/// Force explicitly data rebuild.
				/// </summary>
				/// <param name="kind"></param>
				/// <param name="token"></param>
				void Build(Kind kind, StringToken token) {
					this->kind = kind;
					this->token = token;
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
				Entry(PathName* path, Kind kind, StringToken token) {
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
				/// Checks if this instance belongs to the document's root path.
				/// </summary>
				/// <returns>Boolean</returns>
				bool isRootPath() {
					if (path) {
						return path->IsRoot();
					}
					return path == nullptr;
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
				CommentEntry(){}
				CommentEntry(int c, int l) : index(c), length(l) {}
				~CommentEntry() {
					Marshal::Clear(this, sizeof(*this));
				}
			};
			/// <summary>
			/// Represents the main root contents and container of a TOML document.
			/// </summary>
			class Root {
			public:
				/// <summary>
				/// Factory Initialize
				/// </summary>
				Root(no_init) {
					Contents.Initialize();
					Paths.Initialize();
				}
				Root() = delete;
				/// <summary>
				/// The document original data.
				/// </summary>
				char* Data;
				/// <summary>
				/// The property entries from the original document.
				/// </summary>
				System::Collections::Enumerable<Entry> Contents;
				/// <summary>
				/// Comment entry collection of the global ones from the original document.
				/// </summary>
				System::Collections::Enumerable<CommentEntry> Comments;
				/// <summary>
				/// Path collection that is being pointed by the contents array
				/// [Disposable]
				/// </summary>
				System::Collections::Enumerable<PathName> Paths;
				/// <summary>
				/// [Generation only] Register an entry
				/// </summary>
				/// <param name="entryModelInstance"></param>
				void AddEntry(Entry entryModelInstance) {
					Contents.Push(entryModelInstance);
				}
				/// <summary>
				/// [Generation only] Register an specified entry
				/// </summary>
				/// <param name="entryModelInstance"></param>
				void AddEntry(PathName* path, Kind kind, StringToken token) {
					Entry entry(path, kind, token);
					AddEntry(entry);
				}
				/// <summary>
				///  [Generation only] Register an specified entry (2)
				/// </summary>
				/// <param name="path"></param>
				/// <param name="kind"></param>
				/// <param name="token"></param>
				void AddEntryAndPath(PathName& path, Kind kind, StringToken token) {
					for (int i = 0; i < Paths.getLength(); i++) {
						if (Paths[i].GetContents() == path.GetContents()) {
							AddEntry(&(Paths[i]), kind, token);
							return;
						}
					}
					AddEntry(&Paths.Push(path), kind, token);

				}
				/// <summary>
				/// [Generation only] Register an comment
				/// </summary>
				/// <param name="tokenStart"></param>
				/// <param name="tokenLength"></param>
				void AddComment(int tokenStart, int tokenLength) {
					Comments.Push({ tokenStart, tokenLength });
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
					Contents.~Enumerable();
					Paths.~Enumerable();
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
			/// Encapsulates an dynamic instance of an TOML enumeration
			/// </summary>
			class TOML {
			public:
				/// <summary>
				/// Quick factory initialization
				/// </summary>
				/// <param name="data"></param>
				TOML(char* data) {
					Contents.ConstructorInit();
					Contents->SetData(data);
				}
				TOML() = delete;
				/// <summary>
				/// Dynamic and shared instance to the root container.
				/// </summary>
				System::Instance<Root> Contents;
				/// <summary>
				/// Get the count of the collected entries.
				/// </summary>
				/// <returns></returns>
				size_t Count() {
					if (Contents.NotNull()) {
						return Contents->Contents.getLength();
					}
				}
				/// <summary>
				/// Index accessor
				/// </summary>
				/// <param name="index"></param>
				/// <returns></returns>
				Entry& operator [](size_t index) const {
					return Contents->Contents[index];
				}
				/// <summary>
				/// Get the original data
				/// </summary>
				/// <returns>An nullable char array</returns>
				char* GetText() const {
					return Contents->Data;
				}
				/// <summary>
				/// Destroy this instance
				/// </summary>
				void Destroy() {
					Contents->Destroy();
					Contents.Destroy();
				}
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
				static TOMLResultStatus ParseDecimal(size_t startIndex, char* segmentIterator, Value& output) {
					char* backup = segmentIterator;
					int numerics = 0;
					char* floatValuableBegin = nullptr;
					bool foundDecimalGap = false;
					while (*segmentIterator && *segmentIterator != '#' && *segmentIterator != '\n') {
						Char current = *segmentIterator;
						if ((current.Equals('.') && (!foundDecimalGap)) && numerics == 0){
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
						return Text::couldBeFloatValueRepresentation(buffer, '.', false) ?  Sucess : InvalidFloatFormat;
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
				static TOMLResultStatus ParseNumeric(size_t startIndex, char* segmentIterator, Value& output) {
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
							output.Build(Kind::Integer, StringToken(startIndex, segmentIterator - iterator));
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
				static TOMLResultStatus ParseBoolean(size_t startIndex, char* segmentIterator, Value& output) {
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
								output.Build(Kind::Bool, StringToken(startIndex, length));
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
				static HResult ParseEquation(size_t referenceIndex, char* contents, Value& output, TOMLResultStatus * outputResult) {
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
							output.Build(Kind::String, { referenceIndex, stringAnalysis.Valuable });
							return Sucess;
						}
						else {
							TOMLResultStatus booleanAnalysis = ParseBoolean(referenceIndex, booleanIterator, output);
							*outputResult = booleanAnalysis;
							if (booleanAnalysis.StatusCode == Sucess) {
								return Sucess;
							}
							else {
								TOMLResultStatus integralAnalysis = ParseNumeric(referenceIndex, integerIterator, output);
								*outputResult = integralAnalysis;
								if (integralAnalysis.StatusCode == Sucess) {

									return Sucess;
								}
								else {
									TOMLResultStatus decimalAnalysis = ParseDecimal(referenceIndex, decimalIterator, output);
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
				static Kind ParseLine(size_t& ReferenceIndex, char* line, size_t lineLength, Value& output, TOMLResultStatus * outputStatus) {
					if (!line) {
						return Kind::Unknown;
					}
					while (*line == '#') {
						output.kind = Kind::Comment;
						output.token = StringToken(ReferenceIndex, lineLength);
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
						output.token = StringToken(ReferenceIndex + 1, lineLength - 1);
						return output.kind;
					}
					else {
						HResult h = ParseEquation(ReferenceIndex, line, output, outputStatus);
						if (h == 1 || h == 0) {
							return output.kind;
						}
						return Unknown;
					}
					
				}
				/// <summary>
				/// Recognizes and translates all the values into a readable structure instance.
				/// </summary>
				/// <param name="content">Accessible text data array</param>
				/// <param name="length">Length of the text data array</param>
				/// <returns>[Disposable] Instance of an TOML root.</returns>
				static TOMLHResultOrPtr Parse(char* content, size_t length) {
					char* iterator = content;
					Reader tr{};
					Entry currentEntry;
					PathName currentPath;
					TOML ContentResult	{content};
					bool rootPath = true;
					TOMLResultStatus StatusLine(TOMLResultStatusCode::NotValidTryNext, 0);
					/// INIT RESULT AND MODELS.
					tr.SetContent(content);
					while (!tr.IsEof()) {
						if (rootPath) {
							currentPath.Build();
						}
						/// LAST LINE
						char* cur = tr.Current();
						size_t length = 0;
						/// CURRENT LINE
						char* line = tr.NextLine(length);
						while (*(line + 1) == '\n') {
							line = tr.NextLine(length);
							rootPath = true;
						}
						Value currentVal;
						/// DISTANCE BETWEEN ORIGIN AND CURRENT
						size_t index = cur - content;
						/// PARSE LINE ELEMENT DECLARATION
						auto kind = ParseLine(index, line, length, currentVal, &StatusLine);
						if (kind == Path) {
							rootPath = false;
							/// UPDATE THE CURRENT PATH FOR THIS ELEMENT AND ALL THE NEXT ONES UNTIL DOUBLE LINE BREAK
							currentPath.Build(cur + currentVal.token.getStartIndex(), currentVal.token.getCount());
						}
						else if (kind == Comment) {
							ContentResult.Contents->AddComment(currentVal.token.getStartIndex(), currentVal.token.getCount());
						}
						else if (
							kind == (Kind::String) ||
							kind == (Kind::Array) ||
							kind == (Kind::Bool) ||
							kind == (Kind::Integer)) {

							/// LINKS THE CURRENT PATH TO THE NEW ELEMENT AND ADD IT
							/// IF THE CURRENT PATH DOESNT EXISTS ADDS ITS ALSO
							ContentResult.Contents->AddEntryAndPath(currentPath, kind, currentVal.token);
						}
					}

					
				}
			};
		}
	}
}
