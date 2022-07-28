#include "Lexer.hpp"

struct ProcessException {
    std::string message;
    
    void what() {
        std::cerr << "[Processer Exception]: >> " << message << std::endl;
    }
};

class Processor {
private:
    std::vector<TokenInstance> tokens;
    std::vector<TokenInstance> output;
    std::vector<ProcessorItem> processorItems;

    int tokenOffset = 0;
    int currentProcessor = 0;

    bool atEndOfProcessor() {
        return (currentProcessor >= processorItems.size());
    }

    bool atEndOfTokens(int location) {
        return (location >= tokens.size());
    }

    bool assertProcessorDefinition(std::string name) {
        return (definitions.find(name) != definitions.end());
    }

    void advanceOffset() {
        tokenOffset++;
    }

    void advanceCurrent() {
        currentProcessor++;
    }

    void addProcessorDefinition(std::string name, TokenInstance value) {
        definitions[name] = value;
    }

    ProcessorItem getCurrentProcessor() {
        return (atEndOfProcessor() ? ProcessorItem() : processorItems[currentProcessor]);
    }

    TokenInstance getCurrentToken(int location = -1) {
        location = location == -1 ? tokenOffset : location;
        return (atEndOfTokens(location) ? TokenInstance() : tokens[location]);
    }

    TokenInstance getProcessorDefinition(std::string name) {
        return definitions[name];
    }

    TokenInstance assert(TokenClass token, std::string message) {
        auto current = getCurrentToken();
        
        if (token != getCurrentToken().tokenClass) {
            throw ProcessException { message };
        }

        return current;
    }

    TokenInstance assert(std::vector<TokenClass> token, std::string message) {
        auto current = getCurrentToken();

        for (int i = 0; i < token.size(); i++) {
            if (token[i] == current.tokenClass) {
                return current;
            }
        }

        throw ProcessException { message };
    }

    void define() {
        auto self = getCurrentProcessor();

        self.tokenOffset++;
        tokenOffset = self.tokenOffset;

        auto name = assert(TokenClass::T_IDENTIFIER_LITERAL, "@define expected a name or identifier...");

        advanceOffset();
        assert(TokenClass::T_EQUAL, "@define expected an equal sign for assignment of value...");
        advanceOffset();

        auto value = assert({
            TokenClass::T_NUMBER_LITERAL,
            TokenClass::T_STRING_LITERAL,
            TokenClass::T_TRUE,
            TokenClass::T_FALSE,
            TokenClass::T_NULL
        }, "@define expected a string literal for value...");

        // --> Add the processor definition to the unordered map
        addProcessorDefinition(name.tokenValue, value);
    }

    void ifProcessor() {
        auto self = getCurrentProcessor();
        bool response = false;

        self.tokenOffset++;
        tokenOffset = self.tokenOffset;

        assert(TokenClass::T_LPAREN, "@if expected an '(' before condition of if processor...");
        advanceOffset();

        auto condition = assert({
            TokenClass::T_NUMBER_LITERAL,
            TokenClass::T_STRING_LITERAL,
            TokenClass::T_IDENTIFIER_LITERAL,
            TokenClass::T_TRUE,
            TokenClass::T_FALSE,
            TokenClass::T_NULL
        }, "@if expected a condition bring a valid literal value...");

        advanceOffset();
        assert(TokenClass::T_RPAREN, "@if expected an ')' after condition of if processor...");
        advanceOffset();

        if (condition.tokenClass == TokenClass::T_IDENTIFIER_LITERAL) {
            if (!assertProcessorDefinition(condition.tokenValue))
                throw ProcessException { "@if expected an assigned processor definition name but got '" + condition.tokenValue + "'..." };

            condition = getProcessorDefinition(condition.tokenValue);
        }

        response = (condition.tokenClass != TokenClass::T_FALSE && condition.tokenClass != TokenClass::T_NULL);

        // --> CORE: Now we have a response to check if we should run the processor
        // --> If the response is true, we run the processor and collect those tokens
        // --> If the response is false, we skip the tokens BUT we gather the offset value from the next processor which is expected to be an @else or @end
        
        if (response) {
            auto processor = getCurrentProcessor();

            while (!atEndOfTokens(tokenOffset)) {
                auto token = getCurrentToken();

                if (token.tokenClass == TokenClass::T_IFPROCESSOR || 
                    token.tokenClass == TokenClass::T_ELSEPROCESSOR || 
                    token.tokenClass == TokenClass::T_ENDPROCESSOR) {
                    advanceOffset();
                    break;
                }

                output.push_back(token);
                advanceOffset();
            }

            if (processor.processorClass == TokenClass::T_IFPROCESSOR) {
                ifProcessor();
            }
  
            if (processor.processorClass == TokenClass::T_ENDPROCESSOR) {
                return;
            }

            /*
            if (processor.processorClass == TokenClass::T_ELSEPROCESSOR) {
                int elseOffset = processor.tokenOffset;
                
                advanceCurrent();
                processor = getCurrentProcessor();

                if (processor.processorClass == TokenClass::T_IFPROCESSOR) {
                    ifProcessor();
                    advanceCurrent();
                }

                processor = getCurrentProcessor();

                if (processor.processorClass != TokenClass::T_ENDPROCESSOR)
                    throw ProcessException{"@if expected an @end processor to close block..."};

                int endOffset = processor.tokenOffset;
                int jumpOffset = endOffset - elseOffset;

                tokenOffset += jumpOffset;
            }
            */
        }
    }

public:
    std::unordered_map<std::string, TokenInstance> definitions;

    Processor(std::vector<TokenInstance> tokens, std::vector<ProcessorItem> processorItems) : tokens(tokens), processorItems(processorItems) {}

    std::vector<TokenInstance> process() {
        while (!atEndOfProcessor()) {
            auto processor = getCurrentProcessor();

            switch (processor.processorClass) {
                case TokenClass::T_DEFINEPROCESSOR:
                    define();
                    break;
                case TokenClass::T_IFPROCESSOR:
                    ifProcessor();
                    break;
                default:
                    throw ProcessException { "Invalid processor: " + processor.processorValue };
            }

            advanceCurrent();
        }

        return output;
    }
};