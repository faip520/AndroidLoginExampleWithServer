// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from varnames.djinni

#import "DBVarnameRecord.h"


@implementation DBVarnameRecord

- (nonnull instancetype)initWithField:(int8_t)Field
{
    if (self = [super init]) {
        _Field = Field;
    }
    return self;
}

+ (nonnull instancetype)VarnameRecordWithField:(int8_t)Field
{
    return [(DBVarnameRecord*)[self alloc] initWithField:Field];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@ %p Field:%@>", self.class, (void *)self, @(self.Field)];
}

@end