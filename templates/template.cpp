#include <iostream>
#include <vector>

#include "template.h"

using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

Template::Template(clang::ASTContext& Context) : Analysis(Context) {
}
// step 1: extraction
void Template::extract() {
}
//step 2: compute stats
void Template::analyze(){

}
//step 3: visualization (for later)

// combine
void Template::run(){
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
