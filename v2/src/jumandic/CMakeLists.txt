set(jumandic_headers shared/juman_format.h main/jumanpp.h main/jumanpp_args.h
        shared/jumandic_env.h shared/morph_format.h shared/jumandic_ids.h shared/jumandic_id_resolver.h)
set(jumandic_sources shared/juman_format.cc
        shared/jumandic_env.cc shared/jumandic_test_env.h shared/morph_format.cc shared/jumandic_ids.cc
        shared/jumandic_id_resolver.cc)
set(jumandic_tests shared/jumandic_spec_test.cc shared/mini_dic_test.cc shared/training_test.cc)

set(bug_test_sources tests/bug_950111-003_test.cc)

add_library(jpp_jumandic_spec shared/jumandic_spec.cc shared/jumandic_spec_lexdata.cc shared/jumandic_spec.h)
target_link_libraries(jpp_jumandic_spec jpp_core)

jpp_feature_codegen(
        BASE jpp_jumandic_cg
        CLASS JumandicStatic
        SPEC_INCLUDE shared/jumandic_spec.h
        SPEC_NAME jumanpp::jumandic::SpecFactory
        LINK jpp_jumandic_spec
        DIC_FILE ${JPP_TEST_RESOURCE_DIR}/jumandic/jumanpp_minimal.mdic
)

message(${jpp_jumandic_cg_INCLUDE})

add_library(jpp_jumandic ${jumandic_sources} ${jumandic_headers} ${jpp_jumandic_cg_SRC})
target_include_directories(jpp_jumandic PRIVATE ${jpp_jumandic_cg_INCLUDE})

jpp_test_executable(jpp_jumandic_tests ${jumandic_tests})
jpp_test_executable(jpp_bug_tests ${bug_test_sources})

add_executable(jpp_jumandic_bootstrap main/bootstrap.cc)
add_executable(jumanpp_v2 main/jumanpp.cc main/jumanpp_args.cc)
add_executable(jumanpp_v2_train main/jumanpp_train.cc main/jumanpp_train.h)

target_link_libraries(jpp_jumandic jpp_jumandic_spec)
target_link_libraries(jpp_jumandic_tests jpp_jumandic jpp_core_train)
target_link_libraries(jpp_bug_tests jpp_jumandic jpp_core_train)
target_link_libraries(jpp_jumandic_bootstrap jpp_jumandic)
target_link_libraries(jumanpp_v2 jpp_jumandic)
target_link_libraries(jumanpp_v2_train jpp_jumandic jpp_core_train)

