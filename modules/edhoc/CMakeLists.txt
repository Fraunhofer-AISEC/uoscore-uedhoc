zephyr_interface_library_named(edhoc)
target_include_directories(edhoc INTERFACE inc)
zephyr_library()
zephyr_library_sources(
    src/crypto_wrapper.c
    src/txrx_wrapper.c
    src/byte_array.c
    src/cbor_decoder.c
    src/err_msg.c
    src/initiator.c
    src/print_util.c
    src/responder.c
    src/suites.c
    src/hkdf_info.c
    src/th.c
    src/retrieve_cred.c
    src/okm.c
    src/signature_or_mac_msg.c
    src/prk.c
    src/edhoc_exporter.c
    src/associated_data_encode.c
    src/plaintext.c
    src/edhoc_method_type.c
)

add_definitions(
    -DEDHOC_DEBUG_PRINT
    -DEDHOC_WITH_TINYCRYPT_AND_C25519
)

zephyr_library_link_libraries(edhoc)
target_link_libraries(edhoc INTERFACE zephyr_interface)


