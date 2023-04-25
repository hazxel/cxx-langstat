#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "clang/AST/TemplateBase.h"

#include <nlohmann/json.hpp>

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
using StringRef = llvm::StringRef;

//-----------------------------------------------------------------------------
// Compute statistics on arguments used to instantiate templates,
// no matter whether instantiation are explicit or implicit.
// Should be divided into 3 categories:
// - classes,
// - functions (including member methods), &
// - variables (including class static member variables, (not
//   class field, those cannot be templated if they're not static))
//
// Template instantiations counted should stem from either explicit instantiations
// written by programmers or from implicit ones through 'natural usage'.
//
// Remember that template parameters can be non-types, types or templates.
// Goal: for each instantiation report:
// - classes:
//   * report every explicit instantiation
//   * report every "implicit instantiation", e.g. report ALL instances of
//     std::vector<int>, notably when that occurs multiple times
// - functions:
//   * report every call of f<t> for some instantiating arguments.
//   * explicit function templates are not reported for brevity and due to time,
//     and ease of implementation
// - variables: Report for each var<t> the instantiation, no matter how often
//   that happened.

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis() :
    TemplateInstantiationAnalysis(InstKind::Any, anything(), ".*") {
}

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis(InstKind IK,
    internal::Matcher<clang::NamedDecl> Names, std::string HeaderRegex) :
    IK(IK), Names(Names), HeaderRegex(HeaderRegex) {
        std::cout << "TIA ctor\n";
}

// test code for instantiations:
// - Test code to see if able to find "subinstantiations"
// classTemplateSpecializationDecl(
//     Names,
//     isTemplateInstantiation(),
//     unless(has(cxxRecordDecl())))
// .bind("ImplicitCTSD"),
// - If variable is inside of a template, then it has to be the
// case that the template is being instantiated
// This doesn't work transitively
// anyOf(unless(hasAncestor(functionTemplateDecl())), isInstantiated()),
// - A variable of CTSD type can be matched by this matcher. For
// some reason if it is, it will match twice. Either you uncomment
// the line down below and disallow those variable instantiations
// to be matched here, or you filter out the duplicates below.
// See VarTemplateInstClass.cpp for insights.
// unless(isTemplateInstantiation())

void TemplateInstantiationAnalysis::extractFeatures() {
    // Result of the class insts matcher will give back a pointer to the
    // ClassTemplateSpecializationDecl (CTSD).
    if(IK == InstKind::Class || IK == InstKind::Any){

        // Want variable that has type of some class instantiation,
        // class name is restricted to come from 'Names'
        // WIP: supporting references/pointer to CTSD too.
        auto CTSD = classTemplateSpecializationDecl(
            Names,
            isTemplateInstantiation(),
            isExpansionInFileMatching(HeaderRegex))
            .bind("ImplicitCTSD");
        auto type = hasCanonicalType(hasDeclaration(CTSD));
        // Alternative?
        // hasUnqualifiedDesugaredType(recordType(hasDeclaration(CTSD)))
        auto typematcher = //anyOf(
            hasType(type)/*,
            hasType(references(type)),
            hasType(pointsTo(type)))*/;

        auto ClassInstMatcher = decl(anyOf(
            // -- Implicit --
            // Implicit uses:
            // Variable declarations (which include function parameter variables
            // & static member variables)
            varDecl(
                isExpansionInMainFile(),
                typematcher)
            .bind("VarsFieldThatInstantiateImplicitly"),
            // Field declarations (non static variable member)
            fieldDecl(
                isExpansionInMainFile(),
                typematcher)
            .bind("VarsFieldThatInstantiateImplicitly"),

            // -- Explicit --
            // Explicit instantiations that are not explicit specializations,
            // which is ensured by isTemplateInstantiation() according to
            // matcher reference
            classTemplateSpecializationDecl(
                Names,
                isExpansionInMainFile(), // FIXME: either this or isExpansionInFileMatching with anyOf?
                // should not be stored where classtemplate is stored,
                // because there the implicit instantiations are usually put
                unless(hasParent(classTemplateDecl())),
                isTemplateInstantiation())
            .bind("ExplicitCTSD")));
        auto ClassResults = Extractor.extract2(*Context, ClassInstMatcher);
        ClassExplicitInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
            "ExplicitCTSD");
        ClassImplicitInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
            "ImplicitCTSD");
        // only really needed to find location of where class was implicitly instantiated
        // using variable of member variable/field
        Variables = getASTNodes<DeclaratorDecl>(ClassResults,
            "VarsFieldThatInstantiateImplicitly");

        // No doubt, this is a botch. If some class type was instantiated through
        // a variable templates instantiation, which will be matched twice, the loop
        // below will filter out those variable declarations and the belonging
        // implicit class inst with it in O(n^2). A better solution would be to
        // bundle variables with the class template specialization that is their type
        // and sort (O(nlogn)), but for sake of ease this is (still) omitted.
        // That they're matched twice is due to an bug in RecursiveASTVisitor:
        // https://lists.llvm.org/pipermail/cfe-dev/2021-February/067595.html
        // std::cout << Variables.size() << std::endl;
        for(int i=1; i<Variables.size(); i++){
            for(int j=0; j<i; j++){
                if(i!=j && Variables.at(i) == Variables.at(j)){
                    Variables.erase(Variables.begin()+i);
                    ClassImplicitInsts.erase(ClassImplicitInsts.begin()+i);
                    i--;
                }
            }
        } 
    }

    //
    if(IK == InstKind::Function || IK == InstKind::Any){
        // Old code that matched instantiations no matter explicit/implicit.
        // auto FuncInstMatcher = functionDecl(
        //     isExpansionInMainFile(),
        //     Names,
        //     isTemplateInstantiation())
        // .bind("FuncInsts");
        // Capture all calls that potentially cause an instantiation of a
        // function template. Explicit function template instantation are ignored
        // for ease.
        auto FuncInstMatcher = callExpr(callee(
            functionDecl(
                Names,
                isTemplateInstantiation(),
                isExpansionInFileMatching(HeaderRegex)
            ).bind("FuncInsts")),
            isExpansionInMainFile())
        .bind("callers");
        auto FuncResults = Extractor.extract2(*Context, FuncInstMatcher);
        FuncInsts = getASTNodes<FunctionDecl>(FuncResults, "FuncInsts");
        Callers = getASTNodes<CallExpr>(FuncResults, "callers");
    }

    // Same behavior as with classTemplates: gives pointer to a
    // varSpecializationDecl. However, the location reported is that of the
    // varDecl itself... no matter if explicit or implicit instantiation.
    if(IK == InstKind::Variable || IK == InstKind::Any){
        internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateSpecializationDecl>
            varTemplateSpecializationDecl;
        auto VarInstMatcher = varTemplateSpecializationDecl(
            isExpansionInMainFile(),
            isTemplateInstantiation())
        .bind("VarInsts");
        auto VarResults = Extractor.extract2(*Context, VarInstMatcher);
        VarInsts = getASTNodes<VarTemplateSpecializationDecl>(VarResults,
            "VarInsts");
        if(VarInsts.size())
            removeDuplicateMatches(VarInsts);
    }
}


// Given a mapping from template argumend kind to actual arguments and a given,
// previously unseen argument, check what kind the argument has and add it
// to the mapping.
void updateArgsAndKinds(const TemplateArgument& TArg,
    std::multimap<std::string, std::string>& TArgs) {
        LangOptions LO;
        PrintingPolicy PP(LO);
        PP.PrintCanonicalTypes = true;
        PP.SuppressTagKeyword = true;
        PP.SuppressScope = false;
        PP.SuppressUnwrittenScope = true;
        PP.FullyQualifiedName = true;
        PP.Bool = true;

        std::string Result;
        llvm::raw_string_ostream stream(Result);
        // std::cout << TArg.isPackExpansion() << std::endl;
        // std::cout << TArg.containsUnexpandedParameterPack() << std::endl;

        switch (TArg.getKind()){
            // ** Type **
            case TemplateArgument::Type:
                TArgs.emplace("type", TArg.getAsType().getAsString(PP));
                break;

            // ** Non-type **
            // Declaration
            case TemplateArgument::Declaration:
                cast<clang::NamedDecl>(TArg.getAsDecl())->printQualifiedName(stream, PP);
                TArgs.emplace("non-type", Result);
                break;
            // nullptr
            // FIXME: report type of nullptr, e.g. nullptr to class XY etc.
            case TemplateArgument::NullPtr:
                TArgs.emplace("non-type", "nullptr");
                break;
            // Integers/Integrals
            case TemplateArgument::Integral:
                // just dump that stuff, don't want to deal with llvm::APSInt
                TArg.dump(stream);
                TArgs.emplace("non-type", Result);
                break;

            // ** Template **
            // Template
            case TemplateArgument::Template:
                TArg.getAsTemplate().print(stream, PP);
                TArgs.emplace("template", Result);
                break;
            // TemplateExpansion
            // FIXME: what to do in this case
            case TemplateArgument::TemplateExpansion:
                std::cout << "TemplateExpansion\n";
                break;
            // Pack
            case TemplateArgument::Pack:
                for(auto it=TArg.pack_begin(); it!=TArg.pack_end(); it++)
                    updateArgsAndKinds(*it, TArgs);
                break;

            // ** Miscellaneous **
            // FIXME: what to do in these cases?
            // Expression
            case TemplateArgument::Expression:
                std::cout << "expr\n";
                break;
            // Null
            case TemplateArgument::Null:
                std::cout << "null\n";
                break;
        }
}

// Note about the reported locations:
// For explicit instantiations, a 'single' CTSD match in the AST is returned
// which contains info about the correct location.
// For implicit instantiations (i.e. 'natural' usage e.g. through the use
// of variables, fields), the location of the CTSD is also reported. However,
// since that is a subtree of the tree representing the ClassTemplateDecl, we
// have to do some extra work to get the location of where the instantiation
// in the code actually occurred, that is, the line where the programmer wrote
// the variable or the field.
template<>
unsigned TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<ClassTemplateSpecializationDecl>& Match, bool isImplicit){
    if(isImplicit){
        return Context->getFullLoc(Variables[VariablesCounter++].Node->getInnerLocStart())
            .getLineNumber();
        // can't I just do Variables[i-1].Location to get loc of var/field?
        // std::cout << Variables[i-1].Location << std::endl;
        // return std::to_string(static_cast<int>(Variables[i-1].Location));
    } else{
        return Context->getFullLoc(Match.Node->getTemplateKeywordLoc())
            .getLineNumber();
        // when giving location of explicit inst, can just give match.Location,
        // since CTSD holds right location already since not subtree of CTD
        // return Match.Location;
    }
}

template<>
unsigned TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<FunctionDecl>& Match, bool isImplicit){
        if(isImplicit){
            return Callers[CallersCounter++].Location;
        } else {
            return Context->getFullLoc(Match.Node->getPointOfInstantiation())
                .getLineNumber();
        }
}

template<typename T>
unsigned TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<T>& Match, bool imp){
        return Context->getFullLoc(Match.Node->getPointOfInstantiation())
            .getLineNumber();
}

// Given a vector of matches, create a JSON object storing all instantiations.
template<typename T>
void TemplateInstantiationAnalysis::gatherInstantiationData(Matches<T>& Insts,
    std::string InstKind, bool AreImplicit){
    const std::array<std::string, 3> ArgKinds = {"non-type", "type", "template"};
    ordered_json instances;
    for(auto match : Insts){
        LangOptions LO;
        PrintingPolicy PP(LO);
        PP.PrintCanonicalTypes = true;
        PP.SuppressTagKeyword = true;
        PP.SuppressScope = false;
        PP.SuppressUnwrittenScope = true;
        PP.FullyQualifiedName = true;
        PP.Bool = true;
        std::string DeclName;
        llvm::raw_string_ostream stream(DeclName);
        // FIXME: use match.getDeclName instead
        match.Node->printQualifiedName(stream, PP);
        // std::cout << DeclName << ":" << match.Node->getSpecializationKind() << std::endl;
        // std::cout << DeclName << std::endl;
        std::multimap<std::string, std::string> TArgs;
        const TemplateArgumentList* TALPtr(getTemplateArgs(match));
        // Only report instantiation if it had any arguments it was instantiated
        // with.
        if(TALPtr){
            auto numTArgs = TALPtr->size();
            for(unsigned idx=0; idx<numTArgs; idx++){
                auto TArg = TALPtr->get(idx);
                updateArgsAndKinds(TArg, TArgs);
            }
            ordered_json instance;
            ordered_json arguments;
            instance["location"] = getInstantiationLocation(match, AreImplicit);
            // instance["location"] = match.Location;
            for(auto key : ArgKinds){
                auto range = TArgs.equal_range(key);
                std::vector<std::string> v;
                for (auto it = range.first; it != range.second; it++)
                    v.emplace_back(it->second);
                arguments[key] = v;
            }
            instance["arguments"] = arguments;
            // Use emplace instead of '=' because can be mult. insts for a template
            instances[DeclName].emplace_back(instance);
        } else {
            std::cout << DeclName << " had no inst args\n";
            // FIXME: find more elegant solution
            // No TAL -> skip a function call in reporting -> increase counter
            // to get correct call for each object in FuncInst
            if(InstKind == FuncKey)
                CallersCounter++;
        }
    }
    Features[InstKind] = instances;
}

void TemplateInstantiationAnalysis::analyzeFeatures(){
    extractFeatures();
    if(IK == InstKind::Class || IK == InstKind::Any){
        gatherInstantiationData(ClassExplicitInsts, ExplicitClassKey, false);
        gatherInstantiationData(ClassImplicitInsts, ImplicitClassKey, true);
    }
    if(IK == InstKind::Function || IK == InstKind::Any)
        gatherInstantiationData(FuncInsts, FuncKey, true);
    if(IK == InstKind::Variable || IK == InstKind::Any)
        gatherInstantiationData(VarInsts, VarKey, false);
}

void TemplateInstantiationAnalysis::processFeatures(nlohmann::ordered_json j){
    if(j.contains(ExplicitClassKey)) {
        ordered_json res;
        templatePrevalence(j.at(ExplicitClassKey), res);
        Statistics["tia explicit class insts"] = res;
    }
    if(j.contains(ImplicitClassKey)) {
        ordered_json res;
        templatePrevalence(j.at(ImplicitClassKey), res);
        Statistics["tia implicit class insts"] = res;
    }
    if(j.contains(FuncKey)) {
        ordered_json res;
        templatePrevalence(j.at(FuncKey), res);
        Statistics[FuncKey] = res;
    }
}

//-----------------------------------------------------------------------------
