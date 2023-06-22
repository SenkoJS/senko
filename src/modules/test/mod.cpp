#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <Senkora.hpp>
#include "matchers.hpp"
#include "../modules.hpp"
#include <v8.h>
#include "constants.hpp"

namespace testMod
{
    void describe(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 2)
        {
            Senkora::throwException(ctx, "Expected 2 arguments");
            return;
        }

        if (!args[0]->IsString())
        {
            Senkora::throwException(ctx, "Expected 1st argument to be a string");
            return;
        }

        if (!args[1]->IsFunction())
        {
            Senkora::throwException(ctx, "Expected 2nd argument to be a function");
            return;
        }

        v8::Local<v8::Function> fn = args[1].As<v8::Function>();
        ctx->SetEmbedderData(testConst::getTestEmbedderNum("describe"), args[0]);
        fn->Call(ctx, ctx->Global(), 0, nullptr);
    }

    void test(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 2)
        {
            Senkora::throwException(ctx, "Expected 2 arguments");
            return;
        }

        if (!args[0]->IsString())
        {
            Senkora::throwException(ctx, "Expected 1st argument to be a string");
            return;
        }

        if (!args[1]->IsFunction())
        {
            Senkora::throwException(ctx, "Expected 2nd argument to be a function");
            return;
        }

        v8::Local<v8::Function> fn = args[1].As<v8::Function>();

        // create a new object with the id
        v8::Local<v8::Object> testObj = v8::Object::New(isolate);
        testObj->Set(ctx, v8::String::NewFromUtf8(isolate, "id").ToLocalChecked(), args[0]).Check();

        std::string parentName = *v8::String::Utf8Value(isolate, ctx->GetEmbedderData(158)->ToString(ctx).ToLocalChecked());

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(isolate, true));
        fn->Call(ctx, ctx->Global(), 0, nullptr);

        v8::Local<v8::Value> r = ctx->GetEmbedderData(159);

        if (r->IsBoolean() && r->IsFalse())
        {
            std::string errStr = *v8::String::Utf8Value(isolate, ctx->GetEmbedderData(160)->ToString(ctx).ToLocalChecked());

            printf("%s✗ %s%s > %s%s%s\n", testConst::getColor("red").c_str(), parentName.c_str(), testConst::getColor("reset").c_str(), testConst::getColor("red").c_str(), *v8::String::Utf8Value(isolate, args[0]->ToString(ctx).ToLocalChecked()), testConst::getColor("reset").c_str());
            printf("%s\n", errStr.c_str());
            return;
        }
        else if (r->IsBoolean() && r->IsTrue())
        {
            // check mark, also mention the
            printf("%s✓ %s %s> %s%s%s\n", testConst::getColor("green").c_str(), parentName.c_str(), testConst::getColor("reset").c_str(), testConst::getColor("green").c_str(), *v8::String::Utf8Value(isolate, args[0]->ToString(ctx).ToLocalChecked()), testConst::getColor("reset").c_str());
        }

        args.GetReturnValue().Set(r);
    }

    void expect(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (ctx->GetEmbedderData(159)->IsFalse())
        {
            return;
        }

        v8::Local<v8::Object> expectObj = v8::Object::New(isolate);

        v8::Local<v8::Function> toEqualFn = v8::Function::New(ctx, testMatcher::toEqualCallback)
                                                .ToLocalChecked();

        v8::Local<v8::Function> toBeTrueFn = v8::Function::New(ctx, testMatcher::toBeTrueCallback)
                                                 .ToLocalChecked();

        v8::Local<v8::Function> toBeFalseFn = v8::Function::New(ctx, testMatcher::toBeFalseCallback)
                                                  .ToLocalChecked();

        v8::Local<v8::Function> toBeBooleanFn = v8::Function::New(ctx, testMatcher::toBeBooleanCallback)
                                                    .ToLocalChecked();

        v8::Local<v8::Function> toBeArrayFn = v8::Function::New(ctx, testMatcher::toBeArrayCallback)
                                                  .ToLocalChecked();

        v8::Local<v8::Function> toBeArrayOfSizeFn = v8::Function::New(ctx, testMatcher::toBeArrayOfSizeCallback)
                                                        .ToLocalChecked();

        if (args.Length() != 0 && args.Length() > 1)
        {
            Senkora::throwException(ctx, "Max. allowed: 1 argument");
            return;
        }

        expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "toEqual").ToLocalChecked(), toEqualFn).Check();
        expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "toBeBoolean").ToLocalChecked(), toBeBooleanFn).Check();
        expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "toBeTrue").ToLocalChecked(), toBeTrueFn).Check();
        expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "toBeFalse").ToLocalChecked(), toBeFalseFn).Check();
        expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "toBeArray").ToLocalChecked(), toBeArrayFn).Check();
        expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "toBeArrayOfSize").ToLocalChecked(), toBeArrayOfSizeFn).Check();

        expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "negate").ToLocalChecked(), v8::Boolean::New(isolate, false)).Check();

        if (args.Length() == 1)
        {
            expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "expected").ToLocalChecked(), args[0]).Check();
        }
        else
        {
            expectObj->Set(ctx, v8::String::NewFromUtf8(isolate, "expected").ToLocalChecked(), v8::Undefined(isolate)).Check();
        }

        expectObj->SetAccessor(
                     ctx,
                     v8::String::NewFromUtf8(isolate, "not").ToLocalChecked(),
                     [](v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info)
                     {
                         v8::Isolate *isolate = info.GetIsolate();
                         v8::HandleScope handleScope(isolate);

                         v8::Local<v8::Value> toEqualFn = info.This()->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "toEqual").ToLocalChecked()).ToLocalChecked();

                         // Create a new object with the toEqual function and the negate flag set
                         v8::Local<v8::Object> negatedObj = info.This()->Clone();
                         negatedObj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "negate").ToLocalChecked(), v8::Boolean::New(isolate, true)).Check();

                         info.GetReturnValue().Set(negatedObj);
                     },
                     nullptr,
                     v8::Local<v8::Value>(),
                     v8::DEFAULT,
                     v8::ReadOnly)
            .Check();

        args.GetReturnValue().Set(expectObj);
    }

    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate)
    {
        std::vector<v8::Local<v8::String>> exports;

        exports.push_back(v8::String::NewFromUtf8(isolate, "describe").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "test").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "expect").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "default").ToLocalChecked());

        return exports;
    }

    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::Object> default_exports = v8::Object::New(isolate);

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "describe").ToLocalChecked();
        v8::Local<v8::Value> val = v8::FunctionTemplate::New(isolate, describe)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "test").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, test)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "expect").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, expect)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        Senkora::Modules::setModuleExport(mod, ctx, isolate, v8::String::NewFromUtf8(isolate, "default").ToLocalChecked(), default_exports);

        return v8::Boolean::New(isolate, true);
    }
}