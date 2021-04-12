zephyr_interface_library_named(oscore) target_include_directories(
	oscore INTERFACE inc) zephyr_library()
	zephyr_library_sources(src / crypto_wrapper.c src / aad.c src /
			       oscore2coap.c src / coap2oscore.c src /
			       coap.c src / option.c src / byte_array.c src /
			       security_context.c src / nonce.c src /
			       hkdf_info.c src / oscore_cose.c src /
			       print_util.c src / memcpy_s.c)

		add_definitions(-DOSCORE_DEBUG_PRINT - DOSCORE_WITH_TINYCRYPT)

			zephyr_library_link_libraries(oscore)
				target_link_libraries(
					oscore INTERFACE zephyr_interface)
