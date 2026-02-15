use bmap_rs::bmap;

#[test]
fn test_init_and_dispose() {
    assert!(unsafe { bmap::BMInit() });
    assert!(unsafe { bmap::BMDispose() });
}
