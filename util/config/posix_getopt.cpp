#include "posix_getopt.h"

#include <ctype.h>

#include <util/generic/ptr.h>

namespace NLastGetopt {
    char* optarg;
    int optind;
    int optopt;
    int opterr;
    int optreset;

    static THolder<TOpts> Opts;
    static THolder<TOptsParser> OptsParser;

    int getopt_long_impl(int argc, char* const* argv, const char *optstring,
            const struct option *longopts, int *longindex, bool long_only)
    {
        if (!Opts || optreset == 1) {
            optarg = 0;
            optind = 1;
            opterr = 1;
            optreset = 0;
            Opts.Reset(new TOpts(optstring));

            Opts->AllowSingleDashForLong_ = long_only;

            for (const struct option* o = longopts; o != 0 && o->name != 0; ++o) {
                TOpt* opt;
                if (isalnum(o->val)) {
                    opt = &Opts->CharOption(char(o->val));
                    opt->LongNames_.push_back(o->name);
                } else {
                    Opts->AddLongOption(o->name);
                    opt = const_cast<TOpt*>(&Opts->GetLongOption(o->name));
                }
                opt->HasArg_ = EHasArg(o->has_arg);
                opt->UserValue(o->flag);
            }

            OptsParser.Reset(new TOptsParser(&*Opts, argc, (const char**) argv));
        }

        optarg = 0;

        try {
            if (!OptsParser->Next()) {
                return -1;
            } else {
                optarg = (char*) OptsParser->CurVal();
                optind = (int)OptsParser->Pos_;
                if (!!longindex && !!OptsParser->CurOpt())
                    *longindex = (int) Opts->IndexOf(OptsParser->CurOpt());
                return !!OptsParser->CurOpt() ? OptsParser->CurOpt()->GetCharOr0() : 1;
            }
        } catch (const NLastGetopt::TException&) {
            return '?';
        }
    }

    int getopt_long(int argc, char* const* argv, const char *optstring,
            const struct option *longopts, int *longindex)
    {
        return getopt_long_impl(argc, argv, optstring, longopts, longindex, false);
    }

    int getopt_long_only(int argc, char* const* argv, const char* optstring,
            const struct option *longopts, int *longindex)
    {
        return getopt_long_impl(argc, argv, optstring, longopts, longindex, true);
    }

    // XXX: leading colon is not supported
    // XXX: updating optind by client is not supported
    int getopt(int argc, char* const* argv, const char* optstring) {
        return getopt_long(argc, argv, optstring, 0, 0);
    }

}
