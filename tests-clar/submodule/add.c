#include "clar_libgit2.h"
#include "posix.h"
#include "path.h"
#include "submodule_helpers.h"

static git_repository *g_repo = NULL;

#define SM_LIBGIT2_URL "https://github.com/libgit2/libgit2.git"
#define SM_LIBGIT2     "sm_libgit2"
#define SM_LIBGIT2B    "sm_libgit2b"

void test_submodule_add__cleanup(void)
{
	cl_git_sandbox_cleanup();
}

void test_submodule_add__existing(void)
{
	g_repo = cl_git_sandbox_init("submod2");
	rewrite_gitmodules(git_repository_workdir(g_repo));

	/* re-add existing submodule */
	cl_assert_equal_i(GIT_EEXISTS,
		git_submodule_add_setup(NULL, g_repo, "whatever", "sm_unchanged", 1));
}

static void assert_submodule_url(const char* name, const char *url)
{
	git_config *cfg;
	const char *s;
	git_buf key = GIT_BUF_INIT;

	cl_git_pass(git_repository_config(&cfg, g_repo));

	cl_git_pass(git_buf_printf(&key, "submodule.%s.url", name));
	cl_git_pass(git_config_get_string(&s, cfg, git_buf_cstr(&key)));
	cl_assert_equal_s(s, url);

	git_config_free(cfg);
	git_buf_free(&key);
}

void test_submodule_add__add(void)
{
	git_submodule *sm;

	g_repo = cl_git_sandbox_init("testrepo");

	/* add a submodule using a gitlink */
	cl_git_pass(
		git_submodule_add_setup(&sm, g_repo, SM_LIBGIT2_URL, SM_LIBGIT2, 1)
		);

	cl_assert(git_path_isfile("testrepo/" SM_LIBGIT2 "/.git"));

	cl_assert(git_path_isdir("testrepo/.git/modules"));
	cl_assert(git_path_isdir("testrepo/.git/modules/" SM_LIBGIT2));
	cl_assert(git_path_isfile("testrepo/.git/modules/" SM_LIBGIT2 "/HEAD"));

	assert_submodule_url(SM_LIBGIT2, SM_LIBGIT2_URL);

	/* add a submodule not using a gitlink */

	cl_git_pass(
		git_submodule_add_setup(&sm, g_repo, SM_LIBGIT2_URL, SM_LIBGIT2B, 0)
		);

	cl_assert(git_path_isdir("testrepo/" SM_LIBGIT2B "/.git"));
	cl_assert(git_path_isfile("testrepo/" SM_LIBGIT2B "/.git/HEAD"));
	cl_assert(!git_path_exists("testrepo/.git/modules/" SM_LIBGIT2B));

	assert_submodule_url(SM_LIBGIT2B, SM_LIBGIT2_URL);
}
