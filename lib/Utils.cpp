#include <iostream>

#include "cxx-langstat/Utils.h"


template<typename T>
using StringMap = std::map<std::string, T>;
using clang::TemplateArgumentList;
using clang::ClassTemplateSpecializationDecl;
using clang::VarTemplateSpecializationDecl;
using clang::FunctionDecl;
using clang::CXXMethodDecl;


// Overloaded function to get template arguments depending whether it's a class
// function, or variable.
const TemplateArgumentList*
getTemplateArgs(const Match<ClassTemplateSpecializationDecl>& Match){
    return &(Match.Node->getTemplateInstantiationArgs());
}
const TemplateArgumentList*
getTemplateArgs(const Match<VarTemplateSpecializationDecl>& Match){
    return &(Match.Node->getTemplateInstantiationArgs());
}
// Is this memory-safe?
// Probably yes, assuming the template arguments being stored on the heap,
// being freed only later by the clang library.
const TemplateArgumentList*
getTemplateArgs(const Match<FunctionDecl>& Match){
    // If Match.Node is a non-templated method of a class template
    // we don't care about its instantiation. Then only the class instantiation
    // encompassing it is really interesting, which is output at a different
    // points in code (and time).
    if(auto m = dyn_cast<CXXMethodDecl>(Match.Node)){
        if(m->getInstantiatedFromMemberFunction())
            return nullptr;
    }
    auto TALPtr = Match.Node->getTemplateSpecializationArgs();
    if(!TALPtr){
        std::cerr << "Template argument list ptr is nullptr,"
        << " function declaration " << getMatchDeclName(Match) << " at line " << Match.Location
        << " was not a template specialization" << '\n';
        // should we exit here, or just continue and ignore this failure?
        // did not occur often in my testing so far.
        // happened once when running TIA on Driver.cpp
    }
    return TALPtr;
}

int getNumRelevantTypes(llvm::StringRef Type, const StringMap<int>& SM){
    if(auto [l,r] = Type.split("::__1"); !r.empty())
        return SM.at((l+r).str());
    return SM.at(Type.str());
}

std::string getRelevantTypesAsString(llvm::StringRef Type, nlohmann::json Types,
    const StringMap<int>& SM){
        // std::cout << ContainerType.str() << std::endl;
        int n = getNumRelevantTypes(Type, SM);
        if(n == -1)
            n = Types.end()-Types.begin();
        std::string t;
        for(nlohmann::json::iterator i=Types.begin(); i<Types.begin()+n; i++)
            t = t + (*i).get<std::string>() + ", ";
        if(n)
            return llvm::StringRef(t).drop_back(2).str();
        else
            return "";
}

void templatePrevalence(const nlohmann::ordered_json& in, nlohmann::ordered_json& out){
    std::map<std::string, unsigned> m;
    for(const auto& [Type, Insts] : in.items()){
        // std::cout << Type << std::endl;
        m.try_emplace(Type, Insts.size());
    }
    out = m;
}

void templateTypeArgPrevalence(const nlohmann::ordered_json& in, nlohmann::ordered_json& out,
    const StringMap<int>& SM){
        StringMap<StringMap<unsigned>> m;
        for(const auto& [Type, Insts] : in.items()){
            for(const auto& Inst : Insts){
                m.try_emplace(Type, StringMap<unsigned>());
                nlohmann::json ContainedTypes = Inst["arguments"]["type"];
                assert(ContainedTypes.is_array());
                auto TypeString = getRelevantTypesAsString(Type, ContainedTypes, SM);
                // std::cout << TypeString << std::endl;
                if(!TypeString.empty()){
                    m.at(Type).try_emplace(TypeString, 0);
                    m.at(Type).at(TypeString)++;
                }
            }
        }
        out = m;
}
