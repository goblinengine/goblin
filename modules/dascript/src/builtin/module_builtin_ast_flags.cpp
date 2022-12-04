#include "modules/dascript/src/include/daScript/misc/platform.h"

#include "module_builtin_rtti.h"

#include "modules/dascript/src/include/daScript/simulate/simulate_visit_op.h"
#include "modules/dascript/src/include/daScript/ast/ast_policy_types.h"
#include "modules/dascript/src/include/daScript/ast/ast_expressions.h"
#include "modules/dascript/src/include/daScript/ast/ast_generate.h"
#include "modules/dascript/src/include/daScript/ast/ast_visitor.h"
#include "modules/dascript/src/include/daScript/simulate/aot_builtin_ast.h"
#include "modules/dascript/src/include/daScript/simulate/aot_builtin_string.h"
#include "modules/dascript/src/include/daScript/misc/performance_time.h"

#include "module_builtin_ast.h"

using namespace das;

namespace das {
    TypeDeclPtr makeExprGenFlagsFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprGenFlags";
        ft->argNames = { "alwaysSafe", "generated", "userSaidItsSafe" };
        return ft;
    }

    TypeDeclPtr makeExprFlagsFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprFlags";
        ft->argNames = { "constexpression", "noSideEffects", "noNativeSideEffects", "isForLoopSource", "isCallArgument" };
        return ft;
    }

    TypeDeclPtr makeExprPrintFlagsFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprPrintFlags";
        ft->argNames = { "topLevel", "argLevel", "bottomLevel" };
        return ft;
    }

    TypeDeclPtr makeExprBlockFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprBlockFlags";
        ft->argNames = { "isClosure", "hasReturn", "copyOnReturn", "moveOnReturn",
            "inTheLoop", "finallyBeforeBody", "finallyDisabled","aotSkipMakeBlock",
            "aotDoNotSkipAnnotationData", "isCollapseable", "needCollapse", "hasMakeBlock" };
        return ft;
    }

    TypeDeclPtr makeMakeFieldDeclFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "MakeFieldDeclFlags";
        ft->argNames = { "moveSemantics", "cloneSemantics" };
        return ft;
    }

    TypeDeclPtr makeExprAtFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprAtFlags";
        ft->argNames = { "r2v", "r2cr", "write", "no_promotion" };
        return ft;
    }

    TypeDeclPtr makeExprMakeLocalFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprMakeLocalFlags";
        ft->argNames = { "useStackRef", "useCMRES", "doesNotNeedSp",
            "doesNotNeedInit", "initAllFields" };
        return ft;
    }

    TypeDeclPtr makeExprMakeStructFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprMakeStructFlags";
        ft->argNames = { "useInitializer", "isNewHandle" };
        return ft;
    }

    TypeDeclPtr makeExprAscendFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprAscendFlags";
        ft->argNames = { "useStackRef", "needTypeInfo" };
        return ft;
    }

    TypeDeclPtr makeExprCastFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprCastFlags";
        ft->argNames = { "upcastCast", "reinterpretCast" };
        return ft;
    }
    TypeDeclPtr makeExprVarFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprVarFlags";
        ft->argNames = { "local", "argument", "_block",
            "thisBlock", "r2v", "r2cr", "write" };
        return ft;
    }

   TypeDeclPtr makeExprFieldDerefFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprFieldDerefFlags";
        ft->argNames = { "unsafeDeref", "ignoreCaptureConst" };
        return ft;
    }

    TypeDeclPtr makeExprFieldFieldFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprFieldFieldFlags";
        ft->argNames = { "r2v", "r2cr", "write", "no_promotion" };
        return ft;
    }

    TypeDeclPtr makeExprSwizzleFieldFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprSwizzleFieldFlags";
        ft->argNames = { "r2v", "r2cr", "write" };
        return ft;
    }

    TypeDeclPtr makeExprYieldFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprYieldFlags";
        ft->argNames = { "moveSemantics", "skipLockCheck" };
        return ft;
    }

    TypeDeclPtr makeExprReturnFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprReturnFlags";
        ft->argNames = { "moveSemantics", "returnReference", "returnInBlock", "takeOverRightStack",
        "returnCallCMRES", "returnCMRES", "fromYield", "fromComprehension", "skipLockCheck" };
        return ft;
    }

    TypeDeclPtr makeExprMakeBlockFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "ExprMakeBlockFlags";
        ft->argNames = { "isLambda", "isLocalFunction" };
        return ft;
    }

    TypeDeclPtr makeTypeDeclFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "TypeDeclFlags";
        ft->argNames = { "ref", "constant", "temporary", "_implicit",
            "removeRef", "removeConstant", "removeDim",
            "removeTemporary", "explicitConst", "aotAlias", "smartPtr",
            "smartPtrNative", "isExplicit", "isNativeDim", "isTag" };
        return ft;
    }

    TypeDeclPtr makeFieldDeclarationFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "FieldDeclarationFlags";
        ft->argNames = { "moveSemantics", "parentType", "capturedConstant",
            "generated", "capturedRef", "doNotDelete", "privateField", "sealed", "implemented" };
        return ft;
    }

    TypeDeclPtr makeStructureFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "StructureFlags";
        ft->argNames = { "isClass", "genCtor", "cppLayout", "cppLayoutNotPod",
            "generated", "persistent", "isLambda", "privateStructure",
            "macroInterface", "sealed", "skipLockCheck" };
        return ft;
    }

    TypeDeclPtr makeFunctionFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "FunctionFlags";
        ft->argNames = {
            "builtIn", "policyBased", "callBased", "interopFn", "hasReturn", "copyOnReturn", "moveOnReturn", "exports",
            "init", "addr", "used", "fastCall", "knownSideEffects", "hasToRunAtCompileTime", "unsafeOperation", "unsafeDeref",
            "hasMakeBlock", "aotNeedPrologue", "noAot", "aotHybrid", "aotTemplate", "generated", "privateFunction", "_generator",
            "_lambda", "firstArgReturnType", "noPointerCast", "isClassMethod", "isTypeConstructor", "shutdown", "anyTemplate", "macroInit"
        };
        return ft;
    }

    TypeDeclPtr makeMoreFunctionFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "MoreFunctionFlags";
        ft->argNames = {
            "macroFunction", "needStringCast", "aotHashDeppendsOnArguments", "lateInit", "requestJit", "unsafeOutsideOfFor", "skipLockCheck"
        };
        return ft;
    }

    TypeDeclPtr makeFunctionSideEffectFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "FunctionSideEffectFlags";
        ft->argNames = { "_unsafe", "userScenario","modifyExternal",
            "modifyArgument","accessGlobal","invoke"
        };
        return ft;
    }

    TypeDeclPtr makeVariableFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "VariableFlags";
        ft->argNames = { "init_via_move", "init_via_clone", "used", "aliasCMRES",
            "marked_used", "global_shared", "do_not_delete", "generated",
            "capture_as_ref", "can_shadow", "private_variable", "tag" };
        return ft;
    }

    TypeDeclPtr makeVariableAccessFlags() {
        auto ft = make_smart<TypeDecl>(Type::tBitfield);
        ft->alias = "VariableAccessFlags";
        ft->argNames = { "access_extern", "access_get", "access_ref",
            "access_init", "access_pass" };
        return ft;
    }

    void Module_Ast::registerFlags(ModuleLibrary & ) {
        // FLAGS?
        addAlias(makeTypeDeclFlags());
        addAlias(makeFieldDeclarationFlags());
        addAlias(makeStructureFlags());
        addAlias(makeExprGenFlagsFlags());
        addAlias(makeExprFlagsFlags());
        addAlias(makeExprPrintFlagsFlags());
        addAlias(makeFunctionFlags());
        addAlias(makeMoreFunctionFlags());
        addAlias(makeFunctionSideEffectFlags());
        addAlias(makeVariableFlags());
        addAlias(makeVariableAccessFlags());
        addAlias(makeExprBlockFlags());
        addAlias(makeExprAtFlags());
        addAlias(makeExprMakeLocalFlags());
        addAlias(makeExprAscendFlags());
        addAlias(makeExprCastFlags());
        addAlias(makeExprVarFlags());
        addAlias(makeExprMakeStructFlags());
        addAlias(makeMakeFieldDeclFlags());
        addAlias(makeExprFieldDerefFlags());
        addAlias(makeExprFieldFieldFlags());
        addAlias(makeExprSwizzleFieldFlags());
        addAlias(makeExprYieldFlags());
        addAlias(makeExprReturnFlags());
        addAlias(makeExprMakeBlockFlags());
    }
}
