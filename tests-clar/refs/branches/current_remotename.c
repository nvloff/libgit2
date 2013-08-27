#include "clar_libgit2.h"
#include "refs.h"
#include "branch.h"

git_repository *repo;
static git_buf remote_name;

void test_refs_branches_current_remotename__initialize(void)
{
	git_buf_init(&remote_name, 0);
}

void test_refs_branches_current_remotename__cleanup(void)
{
	git_buf_free(&remote_name);
}

void test_refs_branches_current_remotename__get_current_branch_remote(void)
{
	cl_git_pass(git_repository_open(&repo, cl_fixture("testrepo.git")));

	cl_git_pass(git_branch_current__remote_name(&remote_name, repo));
	cl_assert_equal_s("test", git_buf_cstr(&remote_name));

	git_repository_free(repo);
	repo = NULL;
}

void test_refs_branches_current_remotename__local_detached_broken_head(void)
{
	repo = cl_git_sandbox_init("testrepo.git");

	/* local tracking */
	cl_git_pass(git_repository_set_head(repo, "refs/heads/track-local"));
	cl_git_fail(git_branch_current__remote_name(&remote_name, repo));

	/* detached */
	cl_git_pass(git_repository_set_head(repo, "refs/remotes/test/master"));
	cl_git_fail(git_branch_current__remote_name(&remote_name, repo));

	git_buf_clear(&remote_name);

	/* missing */
	cl_git_pass(git_repository_set_head(repo, "refs/heads/missing_ref"));
	cl_assert_equal_i(GIT_EORPHANEDHEAD, git_branch_current__remote_name(&remote_name, repo));

	cl_git_sandbox_cleanup();
	repo = NULL;
}
