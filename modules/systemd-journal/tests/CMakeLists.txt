add_unit_test(CRITERION
  TARGET test_systemd_journal
  DEPENDS sdjournal
  SOURCES test_systemd_journal.c journald-mock.c test-source.c)
