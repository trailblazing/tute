#ifndef EXPOSE_H
#define EXPOSE_H

#ifndef EXPOSE_INNER_BY_REFERENCE
#define EXPOSE_INNER_BY_REFERENCE(expose_name, inner_name) \
    class expose_name {                                    \
    public:                                                \
        expose_name(inner_name& inRef)                     \
            : ref(inRef)                                   \
        {                                                  \
        }                                                  \
        operator inner_name&() { return ref; }             \
    private:                                               \
        inner_name& ref;                                   \
    };
#endif //EXPOSE_INNER

#endif // EXPOSE_H
