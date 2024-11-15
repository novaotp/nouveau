#include "catch_amalgamated.hpp"
#include "ast.hpp"

TEST_CASE("AST works properly", "[ast]") {
    SECTION("Node Types .toString() methods are correct") {
        REQUIRE(StringType().toString() == "string");
        REQUIRE(IntegerType().toString() == "int");
        REQUIRE(FloatType().toString() == "float");
        REQUIRE(BooleanType().toString() == "bool");
        REQUIRE(VectorType(std::make_shared<BooleanType>(BooleanType())).toString() == "bool[]");
        REQUIRE(FunctionType({}, std::make_shared<BooleanType>(BooleanType())).toString() == "bool ()");
        REQUIRE(
            FunctionType(
                { std::make_shared<StringType>(StringType()) },
                std::make_shared<BooleanType>(BooleanType())
            ).toString() == "bool (string)"
        );
        REQUIRE(
            FunctionType(
                {
                    std::make_shared<StringType>(StringType()),
                    std::make_shared<VectorType>(
                        VectorType(std::make_shared<IntegerType>(IntegerType()))
                    )
                },
                std::make_shared<BooleanType>(BooleanType())
            ).toString() == "bool (string, int[])"
        );
    }

    SECTION("NodeType equality works properly") {
        REQUIRE(StringType().compare(std::make_shared<StringType>()) == true);
        REQUIRE(StringType().compare(std::make_shared<IntegerType>()) == false);

        REQUIRE(
            VectorType(std::make_shared<StringType>())
            .compare(
                std::make_shared<VectorType>(VectorType(std::make_shared<StringType>()))
            ) == true
        );

        REQUIRE(
            VectorType(std::make_shared<StringType>())
            .compare(
                std::make_shared<VectorType>(VectorType(std::make_shared<IntegerType>()))
            ) == false
        );

        REQUIRE(
            VectorType(std::make_shared<StringType>())
            .compare(
                std::make_shared<VectorType>(VectorType(std::make_shared<UnionType>(
                    UnionType(
                        {
                            std::make_shared<FloatType>(),
                            std::make_shared<VectorType>(
                                VectorType(std::make_shared<BooleanType>())
                            )
                        }
                    )
                )))
            ) == false
        );

        REQUIRE(
            OptionalType(std::make_shared<StringType>()).compare(
                std::make_shared<OptionalType>(OptionalType(std::make_shared<StringType>()))
            ) == true);
        REQUIRE(OptionalType(std::make_shared<StringType>()).compare(
            std::make_shared<OptionalType>(OptionalType(std::make_shared<IntegerType>(IntegerType())))
        ) == false);

        REQUIRE(VoidType().compare(std::make_shared<VoidType>()) == true);
        REQUIRE(VoidType().compare(std::make_shared<StringType>()) == false);
    }
}
